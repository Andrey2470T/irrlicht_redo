#include "MainWindow.h"
#include "TimeCounter.h"
#include "Image/Converting.h"
#include "Utils/String.h"
#include "Image/ImageModifier.h"
#include "Utils/String.h"
#include <iostream>

#ifdef _EMSCRIPTEN_
#include <emscripten.h>
#endif

namespace core
{

Clipboard::~Clipboard()
{
    if (ClipboardSelectionText)
        SDL_free(ClipboardSelectionText);
    if (PrimarySelectionText)
        SDL_free(PrimarySelectionText);
}

void Clipboard::copyToClipboard(const c8 *text) const
{
    SDL_SetClipboardText(text);
}

void Clipboard::copyToPrimarySelection(const c8 *text) const
{
#if SDL_VERSION_ATLEAST(2, 25, 0)
    SDL_SetPrimarySelectionText(text);
#endif
}

const c8 *Clipboard::getTextFromClipboard() const
{
    if (ClipboardSelectionText)
        SDL_free(ClipboardSelectionText);
    ClipboardSelectionText = SDL_GetClipboardText();
    return ClipboardSelectionText;
}

const c8 *Clipboard::getTextFromPrimarySelection() const
{
#if SDL_VERSION_ATLEAST(2, 25, 0)
    if (PrimarySelectionText)
        SDL_free(PrimarySelectionText);
    PrimarySelectionText = SDL_GetPrimarySelectionText();
    return PrimarySelectionText;
#endif
    return 0;
}

MainWindow::MainWindow(const MainWindowParameters &params)
    : GLVersion(params.GLType, params.GLVersionMajor, params.GLVersionMinor),
      Cursor(this), Params(params), Resizable(params.Resizable == 1 ? true : false)
{
#ifdef _ANDROID_
	// Blocking on pause causes problems with multiplayer.
	// See https://github.com/minetest/minetest/issues/10842.
	SDL_SetHint(SDL_HINT_ANDROID_BLOCK_ON_PAUSE, "0");
	SDL_SetHint(SDL_HINT_ANDROID_BLOCK_ON_PAUSE_PAUSEAUDIO, "0");

	SDL_SetHint(SDL_HINT_ANDROID_TRAP_BACK_BUTTON, "1");

	// Minetest does its own screen keyboard handling.
	SDL_SetHint(SDL_HINT_ENABLE_SCREEN_KEYBOARD, "0");
#endif

	// Minetest has its own signal handler
	SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");

	// Disabling the compositor is not a good idea in windowed mode.
	// See https://github.com/minetest/minetest/issues/14596
	SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");

#ifdef _COMPILE_WITH_JOYSTICK_EVENTS_
	// These are not interesting for our use
    SDL_SetHint(SDL_HINT_ACCELEROMETER_AS_JOYSTICK, "0");
    SDL_SetHint(SDL_HINT_TV_REMOTE_AS_JOYSTICK, "0");
#endif

#if SDL_VERSION_ATLEAST(2, 24, 0)
	// highdpi support on Windows
	SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING, "1");
#endif

	// Minetest has its own code to synthesize mouse events from touch events,
	// so we prevent SDL from doing it.
	SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
	SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "0");

#ifdef SDL_HINT_APP_NAME
	SDL_SetHint(SDL_HINT_APP_NAME, "Minetest");
#endif

	// Set IME hints
	SDL_SetHint(SDL_HINT_IME_INTERNAL_EDITING, "1");
#ifdef SDL_HINT_IME_SHOW_UI
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

	u32 flags = SDL_INIT_TIMER | SDL_INIT_EVENTS;
	if (Params.InitVideo)
		flags |= SDL_INIT_VIDEO;
#ifdef _COMPILE_WITH_JOYSTICK_EVENTS_
    flags |= SDL_INIT_JOYSTICK;
#endif

    if (SDL_Init(flags) < 0) {
        ErrorStream << "Unable to initialize SDL: " << SDL_GetError() << "\n";
		Close = true;
	} else {
		InfoStream << "SDL initialized\n";
    }

    if (Params.InitVideo)
		if (!initWindow()) {
			Close = true;
			ErrorStream << "Unable to initialize SDL window and context (InitVideo = true)\n";
		}

    GLParams = std::make_unique<GLParameters>(params.GLType, GLVersion);

    if (!GLParams->checkExtensions()) {
        Close = true;
    }

    createKeysMaps();

	SDL_VERSION(&SDLVersion);

    InfoStream << "SDL Version: " << SDLVersion.major << "." <<
        SDLVersion.minor << "." << SDLVersion.patch;

    shouldUpdateViewport = true;
}

MainWindow::~MainWindow()
{
    clearEventQueue();

#ifdef _COMPILE_WITH_JOYSTICK_EVENTS_
    for (auto &joystick : Joysticks)
        if (joystick)
            SDL_JoystickClose(joystick);
#endif

    if (Window && Context) {
        SDL_GL_MakeCurrent(Window, nullptr);
        SDL_GL_DeleteContext(Context);
    }

    if (Window) {
        SDL_DestroyWindow(Window);
    }

	InfoStream << "Quit SDL\n";

    SDL_Quit();
}


// Window params adjusting methods
void MainWindow::setIcon(img::Image *newImg)
{
    if (!Window) return;

    auto localImgMod = new img::ImageModifier();
    auto newImgFlipped = localImgMod->flip(newImg, img::FD_Y);
    delete localImgMod;

    SDL_Surface *surface = img::convertImageToSDLSurface(newImgFlipped);

	if (!surface) {
		ErrorStream << "Failed to create SDL suface\n";
        return;
	}

    SDL_SetWindowIcon(Window, surface);

    delete newImgFlipped;

    surface->pixels = nullptr;
    SDL_FreeSurface(surface);

    return;
}

void MainWindow::setCaption(const std::wstring &newCaption)
{
    std::string utf8_str = utils::wide_to_utf8(newCaption);
    SDL_SetWindowTitle(Window, utf8_str.c_str());
}

bool MainWindow::isActive() const
{
#ifdef _EMSCRIPTEN_
	// Hidden test only does something in some browsers (when tab in background or window is minimized)
	// In other browsers code automatically doesn't seem to be called anymore.
	EmscriptenVisibilityChangeEvent emVisibility;
	if (emscripten_get_visibility_status(&emVisibility) == EMSCRIPTEN_RESULT_SUCCESS) {
		if (emVisibility.hidden)
			return false;
	}
#endif
    const u32 windowFlags = SDL_GetWindowFlags(Window);
    return windowFlags & SDL_WINDOW_SHOWN && windowFlags & SDL_WINDOW_INPUT_FOCUS;
}

bool MainWindow::isFocused() const
{
    return Window && (SDL_GetWindowFlags(Window) & SDL_WINDOW_INPUT_FOCUS) != 0;
}

bool MainWindow::isVisible() const
{
	return !IsInBackground;
}

bool MainWindow::isMinimized() const
{
    return Window && (SDL_GetWindowFlags(Window) & SDL_WINDOW_MINIMIZED) != 0;
}

bool MainWindow::isMaximized() const
{
    return Window && (SDL_GetWindowFlags(Window) & SDL_WINDOW_MAXIMIZED) != 0;
}

bool MainWindow::isFullScreen() const
{
    return Window && ((SDL_GetWindowFlags(Window) & SDL_WINDOW_FULLSCREEN) != 0 ||
        (SDL_GetWindowFlags(Window) & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0);
}

bool MainWindow::isClosed() const
{
    return Close;
}

void MainWindow::close()
{
    InfoStream << "MainWindow::close() Closing the window...\n";
    Close = true;
}

void MainWindow::setResizable(bool resize)
{
#ifdef _EMSCRIPTEN_
	WarnStream << "Resizable not available on the web.\n";
	return;
#else
    if (resize != Resizable) {
		if (Window)
			SDL_SetWindowResizable(Window, (SDL_bool)resize);
		Resizable = resize;
    }
#endif
}

void MainWindow::minimize()
{
    if (Window)
        SDL_MinimizeWindow(Window);
}

void MainWindow::maximize()
{
    if (Window)
        SDL_MaximizeWindow(Window);
}

void MainWindow::restore()
{
    if (Window)
        SDL_RestoreWindow(Window);
}

bool MainWindow::setFullscreen(bool fullscreen)
{
    if (!Window)
		return false;
	// The SDL wiki says that this may trigger SDL_RENDER_TARGETS_RESET, but
	// looking at the SDL source, this only happens with D3D, so it's not
	// relevant to us.
	bool success = SDL_SetWindowFullscreen(Window, getFullscreenFlag(fullscreen)) == 0;
    if (!success) {
        ErrorStream << "SDL_SetWindowFullscreen failed: " << SDL_GetError() << "\n";
    }
    return success;
}

void MainWindow::SwapWindow()
{
    glFlush();

    if (Window)
        SDL_GL_SwapWindow(Window);
}

bool MainWindow::isUsingWayland() const
{
    if (!Window)
        return false;
    auto *name = SDL_GetCurrentVideoDriver();
    return name && !strcmp(name, "wayland");
}

bool MainWindow::activateJoysticks(std::vector<JoystickInfo> &joysticksInfo)
{
#ifdef _COMPILE_WITH_JOYSTICK_EVENTS_
    joysticksInfo.clear();

    // we can name up to 256 different joysticks
    const int numJoysticks = std::min(SDL_NumJoysticks(), 256);
    Joysticks.resize(numJoysticks);
    joysticksInfo.resize(numJoysticks);

    for (int j = 0; j < numJoysticks; ++j) {
        Joysticks.push_back(SDL_JoystickOpen(j));
        JoystickInfo info;

        info.Joystick = j;
        info.Axes = SDL_JoystickNumAxes(Joysticks[j]);
        info.Buttons = SDL_JoystickNumButtons(Joysticks[j]);
        info.Name = SDL_JoystickName(Joysticks[j]);
        info.PovHat = (SDL_JoystickNumHats(Joysticks[j]) > 0)
                          ? JoystickInfo::POV_HAT_PRESENT
                          : JoystickInfo::POV_HAT_ABSENT;

        joysticksInfo.push_back(info);
    }

    return true;

#endif

    return false;
}

std::optional<Event> MainWindow::popEvent()
{
    if (Events.empty())
        return std::nullopt;
    auto event = Events.front();
    Events.pop();

    return event;
}

void MainWindow::clearEventQueue()
{
    while (!Events.empty()) {
        auto event = Events.front();
        if (event.Type == ET_STRING_INPUT_EVENT && event.StringInput.Str) {
            delete event.StringInput.Str;
        }
        Events.pop();
    }
}

bool MainWindow::pollEventsFromQueue()
{
    SDL_Event sdlevent;

    Event irrevent;
    while (!Close && SDL_PollEvent(&sdlevent)) {
        switch (sdlevent.type) {
        case SDL_MOUSEMOTION: {
            SDL_Keymod keymod = SDL_GetModState();

            irrevent.Type = ET_MOUSE_INPUT_EVENT;
            irrevent.MouseInput.Type = MIE_MOUSE_MOVED;

            MouseRelPos.X = static_cast<s32>(sdlevent.motion.xrel * Scale.X);
            MouseRelPos.Y = static_cast<s32>(sdlevent.motion.yrel * Scale.Y);
            if (!SDL_GetRelativeMouseMode()) {
                MousePos.X = static_cast<s32>(sdlevent.motion.x * Scale.X);
                MousePos.Y = static_cast<s32>(sdlevent.motion.y * Scale.Y);
            } else {
                MousePos.X += MouseRelPos.X;
                MousePos.Y += MouseRelPos.Y;
            }
            irrevent.MouseInput.X = MousePos.X;
            irrevent.MouseInput.Y = MousePos.Y;

            irrevent.MouseInput.ButtonStates = MouseButtonStates;
            irrevent.MouseInput.Shift = (keymod & KMOD_SHIFT) != 0;
            irrevent.MouseInput.Control = (keymod & KMOD_CTRL) != 0;

            Events.push(irrevent);

            break;
        }
        case SDL_MOUSEWHEEL: {
            SDL_Keymod keymod = SDL_GetModState();

            irrevent.Type = ET_MOUSE_INPUT_EVENT;
            irrevent.MouseInput.Type = MIE_MOUSE_WHEEL;
#if SDL_VERSION_ATLEAST(2, 0, 18)
            irrevent.MouseInput.WheelDelta = sdlevent.wheel.preciseY;
#else
            irrevent->MouseInput.Wheel = sdlevent.wheel.y;
#endif
            irrevent.MouseInput.ButtonStates = MouseButtonStates;
            irrevent.MouseInput.Shift = (keymod & KMOD_SHIFT) != 0;
            irrevent.MouseInput.Control = (keymod & KMOD_CTRL) != 0;
            irrevent.MouseInput.X = MousePos.X;
            irrevent.MouseInput.Y = MousePos.Y;

            Events.push(irrevent);

            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
            SDL_Keymod keymod = SDL_GetModState();

            irrevent.Type = ET_MOUSE_INPUT_EVENT;
            irrevent.MouseInput.X = static_cast<s32>(sdlevent.button.x * Scale.X);
            irrevent.MouseInput.Y = static_cast<s32>(sdlevent.button.y * Scale.Y);
            irrevent.MouseInput.Shift = (keymod & KMOD_SHIFT) != 0;
            irrevent.MouseInput.Control = (keymod & KMOD_CTRL) != 0;

            irrevent.MouseInput.Type = MIE_MOUSE_MOVED;

#ifdef _EMSCRIPTEN_
            // Handle mouselocking in emscripten in Windowed mode.
            // In fullscreen SDL will handle it.
            // The behavior we want windowed is - when the canvas was clicked then
            // we will lock the mouse-pointer if it should be invisible.
            // For security reasons this will be delayed until the next mouse-up event.
            // We do not pass on this event as we don't want the activation click to do anything.
            if (sdlevent.type == SDL_MOUSEBUTTONDOWN && !isFullscreen()) {
                EmscriptenPointerlockChangeEvent pointerlockStatus; // let's hope that test is not expensive ...
                if (emscripten_get_pointerlock_status(&pointerlockStatus) == EMSCRIPTEN_RESULT_SUCCESS) {
                    if (Cursor.isVisible() && pointerlockStatus.isActive) {
                        emscripten_exit_pointerlock();
                        return !Close;
                    } else if (!Cursor.isVisible() && !pointerlockStatus.isActive) {
                        emscripten_request_pointerlock(0, true);
                        return !Close;
                    }
                }
            }
#endif

            auto button = sdlevent.button.button;
#ifdef _ANDROID_
            // Android likes to send the right mouse button as the back button.
            // According to some web searches I did, this is probably
            // vendor/device-specific.
            // Since a working right mouse button is very important for
            // Minetest, we have this little hack.
            if (button == SDL_BUTTON_X2)
                button = SDL_BUTTON_RIGHT;
#endif
            switch (button) {
            case SDL_BUTTON_LEFT:
                if (sdlevent.type == SDL_MOUSEBUTTONDOWN) {
                    irrevent.MouseInput.Type = MIE_LMOUSE_PRESSED_DOWN;
                    MouseButtonStates |= MBSM_LEFT;
                } else {
                    irrevent.MouseInput.Type = MIE_LMOUSE_LEFT_UP;
                    MouseButtonStates &= ~MBSM_LEFT;
                }
                break;

            case SDL_BUTTON_RIGHT:
                if (sdlevent.type == SDL_MOUSEBUTTONDOWN) {
                    irrevent.MouseInput.Type = MIE_RMOUSE_PRESSED_DOWN;
                    MouseButtonStates |= MBSM_RIGHT;
                } else {
                    irrevent.MouseInput.Type = MIE_RMOUSE_LEFT_UP;
                    MouseButtonStates &= ~MBSM_RIGHT;
                }
                break;

            case SDL_BUTTON_MIDDLE:
                if (sdlevent.type == SDL_MOUSEBUTTONDOWN) {
                    irrevent.MouseInput.Type = MIE_MMOUSE_PRESSED_DOWN;
                    MouseButtonStates |= MBSM_MIDDLE;
                } else {
                    irrevent.MouseInput.Type = MIE_MMOUSE_LEFT_UP;
                    MouseButtonStates &= ~MBSM_MIDDLE;
                }
                break;
            }

            irrevent.MouseInput.ButtonStates = MouseButtonStates;

            if (irrevent.MouseInput.Type != MIE_MOUSE_MOVED) {
                if (irrevent.MouseInput.Type >= MIE_LMOUSE_PRESSED_DOWN && irrevent.MouseInput.Type <= MIE_MMOUSE_PRESSED_DOWN) {
                    u32 clicks = checkSuccessiveClicks(irrevent.MouseInput.X, irrevent.MouseInput.Y, irrevent.MouseInput.Type);
                    if (clicks == 2) {
                        irrevent.MouseInput.Type = (MouseInputEventType)(MIE_LMOUSE_DOUBLE_CLICK + irrevent.MouseInput.Type - MIE_LMOUSE_PRESSED_DOWN);
                    } else if (clicks == 3) {
                        irrevent.MouseInput.Type = (MouseInputEventType)(MIE_LMOUSE_TRIPLE_CLICK + irrevent.MouseInput.Type - MIE_LMOUSE_PRESSED_DOWN);
                    }
                }

                Events.push(irrevent);
            }

            break;
        }

        case SDL_TEXTINPUT: {
            irrevent.Type = ET_STRING_INPUT_EVENT;
            irrevent.StringInput.Str = new std::wstring();
            *(irrevent.StringInput.Str) = utf8_to_wide(sdlevent.text.text);
            Events.push(irrevent);
        } break;

        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            KeysMap mp;
            mp.SDLKey = sdlevent.key.keysym.sym;
            s32 idx = std::distance(KeysMaps.begin(), std::lower_bound(KeysMaps.begin(), KeysMaps.end(), mp));

            KEY_CODE key;
            if (idx == -1)
                key = (KEY_CODE)0;
            else
                key = (KEY_CODE)KeysMaps.at(idx).Win32Key;

            if (key == (KEY_CODE)0)
                InfoStream << "MainWindow::pollEventsFromQueue(): The key code " << (s32)mp.SDLKey << " is not mapped\n";

            // Make sure to only input special characters if something is in focus, as SDL_TEXTINPUT handles normal unicode already
            if (SDL_IsTextInputActive() && !keyIsKnownSpecial(key) && (sdlevent.key.keysym.mod & KMOD_CTRL) == 0)
                break;

            irrevent.Type = ET_KEY_INPUT_EVENT;
            irrevent.KeyInput.Key = key;
            irrevent.KeyInput.PressedDown = (sdlevent.type == SDL_KEYDOWN);
            irrevent.KeyInput.Shift = (sdlevent.key.keysym.mod & KMOD_SHIFT) != 0;
            irrevent.KeyInput.Control = (sdlevent.key.keysym.mod & KMOD_CTRL) != 0;
            irrevent.KeyInput.Char = findCharToPassToIrrlicht(mp.SDLKey, key,
                                                              (sdlevent.key.keysym.mod & KMOD_NUM) != 0);
            Events.push(irrevent);

        } break;

        case SDL_QUIT: {
            InfoStream << "MainWindow::pollEventsFromQueue(): SDL_QUIT\n";
            Close = true;
            break;
        }

        case SDL_WINDOWEVENT:
            switch (sdlevent.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
            case SDL_WINDOWEVENT_SIZE_CHANGED:
#if SDL_VERSION_ATLEAST(2, 0, 18)
            case SDL_WINDOWEVENT_DISPLAY_CHANGED:
#endif
                u32 old_w = Params.Width, old_h = Params.Height;
                f32 old_scale_x = Scale.X, old_scale_y = Scale.Y;
                updateViewportAndScale();
                if (old_w != Params.Width || old_h != Params.Height) {
                    shouldUpdateViewport = true;
                }
                if (old_scale_x != Scale.X || old_scale_y != Scale.Y) {
                    irrevent.Type = ET_APPLICATION_EVENT;
                    irrevent.Application.Type = AET_DPI_CHANGED;
                    Events.push(irrevent);
                }
                break;
            }
            break;

        case SDL_USEREVENT:
            irrevent.Type = ET_USER_EVENT;
            irrevent.User.UserData1 = reinterpret_cast<uintptr_t>(sdlevent.user.data1);
            irrevent.User.UserData2 = reinterpret_cast<uintptr_t>(sdlevent.user.data2);

            Events.push(irrevent);
            break;

        case SDL_FINGERDOWN:
            irrevent.Type = ET_TOUCH_INPUT_EVENT;
            irrevent.TouchInput.Type = TIE_PRESSED_DOWN;
            irrevent.TouchInput.ID = sdlevent.tfinger.fingerId;
            irrevent.TouchInput.X = static_cast<s32>(sdlevent.tfinger.x * Params.Width);
            irrevent.TouchInput.Y = static_cast<s32>(sdlevent.tfinger.y * Params.Height);
            CurrentTouchCount++;
            irrevent.TouchInput.touchedCount = CurrentTouchCount;

            Events.push(irrevent);
            break;

        case SDL_FINGERMOTION:
            irrevent.Type = ET_TOUCH_INPUT_EVENT;
            irrevent.TouchInput.Type = TIE_MOVED;
            irrevent.TouchInput.ID = sdlevent.tfinger.fingerId;
            irrevent.TouchInput.X = static_cast<s32>(sdlevent.tfinger.x * Params.Width);
            irrevent.TouchInput.Y = static_cast<s32>(sdlevent.tfinger.y * Params.Height);
            irrevent.TouchInput.touchedCount = CurrentTouchCount;

            Events.push(irrevent);
            break;

        case SDL_FINGERUP:
            irrevent.Type = ET_TOUCH_INPUT_EVENT;
            irrevent.TouchInput.Type = TIE_LEFT_UP;
            irrevent.TouchInput.ID = sdlevent.tfinger.fingerId;
            irrevent.TouchInput.X = static_cast<s32>(sdlevent.tfinger.x * Params.Width);
            irrevent.TouchInput.Y = static_cast<s32>(sdlevent.tfinger.y * Params.Height);
            // To match Android behavior, still count the pointer that was
            // just released.
            irrevent.TouchInput.touchedCount = CurrentTouchCount;
            if (CurrentTouchCount > 0) {
                CurrentTouchCount--;
            }

            Events.push(irrevent);
            break;

            // Contrary to what the SDL documentation says, SDL_APP_WILLENTERBACKGROUND
            // and SDL_APP_WILLENTERFOREGROUND are actually sent in onStop/onStart,
            // not onPause/onResume, on recent Android versions. This can be verified
            // by testing or by looking at the org.libsdl.app.SDLActivity Java code.
            // -> This means we can use them to implement isWindowVisible().

        case SDL_APP_WILLENTERBACKGROUND:
            IsInBackground = true;
            break;

        case SDL_APP_WILLENTERFOREGROUND:
            IsInBackground = false;
            break;

        case SDL_RENDER_TARGETS_RESET:
            ErrorStream << "MainWindow::pollEventsFromQueue() Received SDL_RENDER_TARGETS_RESET. Rendering is probably broken.\n";
            break;

        case SDL_RENDER_DEVICE_RESET:
            ErrorStream << "MainWindow::pollEventsFromQueue() Received SDL_RENDER_DEVICE_RESET. Rendering is probably broken.\n";
            break;

        default:
            break;
        } // end switch
    } // end while

#ifdef _COMPILE_WITH_JOYSTICK_EVENTS_
    // TODO: Check if the multiple open/close calls are too expensive, then
    // open/close in the constructor/destructor instead

    // update joystick states manually
    SDL_JoystickUpdate();
    // we'll always send joystick input events...
    Event joyevent;
    joyevent.Type = ET_JOYSTICK_INPUT_EVENT;
    for (u32 i = 0; i < Joysticks.size(); ++i) {
        SDL_Joystick *joystick = Joysticks[i];
        if (joystick) {
            int j;
            // query all buttons
            const int numButtons = std::min(SDL_JoystickNumButtons(joystick), 32);
            joyevent.Joystick.ButtonStates = 0;
            for (j = 0; j < numButtons; ++j)
                joyevent.Joystick.ButtonStates |= (SDL_JoystickGetButton(joystick, j) << j);

            // query all axes, already in correct range
            const int numAxes = std::min(SDL_JoystickNumAxes(joystick), (int)Event::JoystickEvent::NUMBER_OF_AXES);
            joyevent.Joystick.Axis[Event::JoystickEvent::AXIS_X] = 0;
            joyevent.Joystick.Axis[Event::JoystickEvent::AXIS_Y] = 0;
            joyevent.Joystick.Axis[Event::JoystickEvent::AXIS_Z] = 0;
            joyevent.Joystick.Axis[Event::JoystickEvent::AXIS_R] = 0;
            joyevent.Joystick.Axis[Event::JoystickEvent::AXIS_U] = 0;
            joyevent.Joystick.Axis[Event::JoystickEvent::AXIS_V] = 0;
            for (j = 0; j < numAxes; ++j)
                joyevent.Joystick.Axis[j] = SDL_JoystickGetAxis(joystick, j);

            // we can only query one hat, SDL only supports 8 directions
            if (SDL_JoystickNumHats(joystick) > 0) {
                switch (SDL_JoystickGetHat(joystick, 0)) {
                case SDL_HAT_UP:
                    joyevent.Joystick.POV = 0;
                    break;
                case SDL_HAT_RIGHTUP:
                    joyevent.Joystick.POV = 4500;
                    break;
                case SDL_HAT_RIGHT:
                    joyevent.Joystick.POV = 9000;
                    break;
                case SDL_HAT_RIGHTDOWN:
                    joyevent.Joystick.POV = 13500;
                    break;
                case SDL_HAT_DOWN:
                    joyevent.Joystick.POV = 18000;
                    break;
                case SDL_HAT_LEFTDOWN:
                    joyevent.Joystick.POV = 22500;
                    break;
                case SDL_HAT_LEFT:
                    joyevent.Joystick.POV = 27000;
                    break;
                case SDL_HAT_LEFTUP:
                    joyevent.Joystick.POV = 31500;
                    break;
                case SDL_HAT_CENTERED:
                default:
                    joyevent.Joystick.POV = 65535;
                    break;
                }
            } else {
                joyevent.Joystick.POV = 65535;
            }

            // we map the number directly
            joyevent.Joystick.Joystick = static_cast<u8>(i);
            // now post the event
            Events.push(joyevent);
            // and close the joystick
        }
    }
#endif
    return !Close;
}

#ifdef _EMSCRIPTEN_
EM_BOOL MainWindow::MouseUpDownCallback(int eventType, const EmscriptenMouseEvent *event, void *userData)
{
    // We need this callback so far only because otherwise "emscripten_request_pointerlock" calls will
    // fail as their request are infinitely deferred.
    // Not exactly certain why, maybe SDL does catch those mouse-events otherwise and not pass them on.
    return EM_FALSE;
}

EM_BOOL MainWindow::MouseEnterCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    MainWindow *This = static_cast<MainWindow *>(userData);

    Event event;

    event.Type = EET_MOUSE_INPUT_EVENT;
    event.MouseInput.Event = EMIE_MOUSE_ENTER_CANVAS;
    This->MouseX = event.MouseInput.X = mouseEvent->canvasX;
    This->MouseY = event.MouseInput.Y = mouseEvent->canvasY;
    This->MouseXRel = mouseEvent->movementX; // should be 0 I guess? Or can it enter while pointer is locked()?
    This->MouseYRel = mouseEvent->movementY;
    event.MouseInput.ButtonStates = This->MouseButtonStates; // TODO: not correct, but couldn't figure out the bitset of mouseEvent->buttons yet.
    event.MouseInput.Shift = mouseEvent->shiftKey;
    event.MouseInput.Control = mouseEvent->ctrlKey;

    This->postEventFromUser(event);

    return EM_FALSE;
}

EM_BOOL MainWindow::MouseLeaveCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    MainWindow *This = static_cast<MainWindow *>(userData);

    Event event;

    event.Type = EET_MOUSE_INPUT_EVENT;
    event.MouseInput.Event = EMIE_MOUSE_LEAVE_CANVAS;
    This->MouseX = event.MouseInput.X = mouseEvent->canvasX;
    This->MouseY = event.MouseInput.Y = mouseEvent->canvasY;
    This->MouseXRel = mouseEvent->movementX; // should be 0 I guess? Or can it enter while pointer is locked()?
    This->MouseYRel = mouseEvent->movementY;
    event.MouseInput.ButtonStates = This->MouseButtonStates; // TODO: not correct, but couldn't figure out the bitset of mouseEvent->buttons yet.
    event.MouseInput.Shift = mouseEvent->shiftKey;
    event.MouseInput.Control = mouseEvent->ctrlKey;

    This->postEventFromUser(event);

    return EM_FALSE;
}
#endif

std::string MainWindow::getVendorName() const
{
    std::string vendorStr = (const char*)GLParams->vendor;
    return vendorStr;
}

std::string MainWindow::getGLVersionString() const
{
    std::string versionStr = (const char*)GLParams->version;
    return "OpenGL" + versionStr;;
}

v2u MainWindow::getWindowSize() const
{
    int width, height;
    SDL_GetWindowSize(Window, &width, &height);
	
    return v2u((u32)width, (u32)height);
}

v2u MainWindow::getViewportSize() const
{
    int width, height;
    SDL_GL_GetDrawableSize(Window, &width, &height);
    return v2u((u32)width, (u32)height);
}

u32 MainWindow::getFullscreenFlag(bool fullscreen)
{
	if (!fullscreen)
		return 0;
#ifdef _EMSCRIPTEN_
	return SDL_WINDOW_FULLSCREEN;
#else
	return SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif
}

f32 MainWindow::getDisplayDensity() const
{
    return std::max(Scale.X * 96.0f, Scale.Y * 96.0f);;
}

#define DEFAULT_DPI 96.0
#define POINTS_PER_INCH 72.0

u32 MainWindow::getScreenDPI() const
{
    f32 ddpi, hdpi, vdpi;
    if (SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi) == 0) {
        return (hdpi + vdpi) / 2.0f;
    }
    return DEFAULT_DPI; // Fallback
}

const OpenGLVersion &MainWindow::getOpenGLVersion() const
{
    return GLVersion;
}

const GLParameters *MainWindow::getGLParams() const
{
    return GLParams.get();
}

SDL_GLprofile MainWindow::convertIrrGLTypeToProfile() const
{
    switch (GLVersion.Type) {
    case OGL_TYPE_DESKTOP:
        return SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
    case OGL_TYPE_ES:
    case OGL_TYPE_WEB:
        return SDL_GL_CONTEXT_PROFILE_ES;
    }

    return SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
}

const Clipboard *MainWindow::getClipboard() const
{
    return &SDLClipboard;
}

CursorControl &MainWindow::getCursorControl()
{
    return Cursor;
}

void MainWindow::updateViewportAndScale()
{
    int window_w, window_h;
	SDL_GetWindowSize(Window, &window_w, &window_h);

    int drawable_w, drawable_h;
    SDL_GL_GetDrawableSize(Window, &drawable_w, &drawable_h);

	Scale = utils::v2f(
        (float)drawable_w / (float)window_w,
        (float)drawable_h / (float)window_h);

    Params.Width = drawable_w;
    Params.Height = drawable_h;
}

bool MainWindow::initWindow()
{
    u32 Wnd_Flags = 0;
	Wnd_Flags |= SDL_WINDOW_ALLOW_HIGHDPI;

	Wnd_Flags |= getFullscreenFlag(Params.FullScreen);
	if (Resizable)
		Wnd_Flags |= SDL_WINDOW_RESIZABLE;
	if (Params.Maximized)
		Wnd_Flags |= SDL_WINDOW_MAXIMIZED;
	Wnd_Flags |= SDL_WINDOW_OPENGL;

	SDL_GL_ResetAttributes();

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, Params.ColorChannelBits);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, Params.ColorChannelBits);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, Params.ColorChannelBits);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, Params.DepthBits);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, Params.StencilBits);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, Params.SwapBuffers ? 1 : 0);

	if (Params.AntiAlias > 1) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, Params.AntiAlias);
	} else {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
	}

#ifdef _EMSCRIPTEN_
	if (Params.Width != 0 || Params.Height != 0)
		emscripten_set_canvas_size(Params.Width, Params.Height);
	else
		emscripten_get_canvas_size(&Params.Width, &Params.Height, nullptr);

	SDL_CreateWindowAndRenderer(0, 0, Wnd_Flags, &Window, &Renderer); // 0,0 will use the canvas size

	if (!Window || !Renderer) {
		ErrorStream << "Could not create window or renderer: " << SDL_GetError() << "\n";
		Close = true;
		return false;
	}

	// "#canvas" is for the opengl context
	emscripten_set_mousedown_callback("#canvas", (void *)this, true, MouseUpDownCallback);
	emscripten_set_mouseup_callback("#canvas", (void *)this, true, MouseUpDownCallback);
	emscripten_set_mouseenter_callback("#canvas", (void *)this, false, MouseEnterCallback);
	emscripten_set_mouseleave_callback("#canvas", (void *)this, false, MouseLeaveCallback);
#else
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GLVersion.Major);
   // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GLVersion.Minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, convertIrrGLTypeToProfile());

	if (Params.DriverDebug)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG);

    Window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Params.Width, Params.Height, Wnd_Flags);

	if (!Window) {
        ErrorStream << "Could not create window: " << SDL_GetError() << "\n";
		Close = true;
		return false;
    }

    Context = SDL_GL_CreateContext(Window);
    if (!Context) {
        ErrorStream << "Could not create context: " << SDL_GetError() << "\n";
		Close = true;
		return false;
    }

    updateViewportAndScale();
	if (Scale.X != 1.0f || Scale.Y != 1.0f) {
		// The given window size is in pixels, not in screen coordinates.
		// We can only do the conversion now since we didn't know the scale before.
		SDL_SetWindowSize(Window,
				static_cast<int>(Params.Width / Scale.X),
				static_cast<int>(Params.Height / Scale.Y));
		// Re-center, otherwise large, non-maximized windows go offscreen.
		SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		updateViewportAndScale();
    }
#endif

    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        std::cerr << "Could not initialize GLEW: " << glewGetErrorString(glewStatus) << std::endl;
        Close = true;
        return false;
    }

    return true;
}

void MainWindow::createKeysMaps()
{
    // Android back button = ESC
    KeysMaps.emplace_back(SDLK_AC_BACK, KEY_ESCAPE);

    KeysMaps.emplace_back(SDLK_BACKSPACE, KEY_BACK);
    KeysMaps.emplace_back(SDLK_TAB, KEY_TAB);
    KeysMaps.emplace_back(SDLK_CLEAR, KEY_CLEAR);
    KeysMaps.emplace_back(SDLK_RETURN, KEY_RETURN);

    // combined modifiers missing

    KeysMaps.emplace_back(SDLK_PAUSE, KEY_PAUSE);
    KeysMaps.emplace_back(SDLK_CAPSLOCK, KEY_CAPITAL);

    // asian letter keys missing

    KeysMaps.emplace_back(SDLK_ESCAPE, KEY_ESCAPE);

    // asian letter keys missing

    KeysMaps.emplace_back(SDLK_SPACE, KEY_SPACE);
    KeysMaps.emplace_back(SDLK_PAGEUP, KEY_PRIOR);
    KeysMaps.emplace_back(SDLK_PAGEDOWN, KEY_NEXT);
    KeysMaps.emplace_back(SDLK_END, KEY_END);
    KeysMaps.emplace_back(SDLK_HOME, KEY_HOME);
    KeysMaps.emplace_back(SDLK_LEFT, KEY_LEFT);
    KeysMaps.emplace_back(SDLK_UP, KEY_UP);
    KeysMaps.emplace_back(SDLK_RIGHT, KEY_RIGHT);
    KeysMaps.emplace_back(SDLK_DOWN, KEY_DOWN);

    // select missing
    KeysMaps.emplace_back(SDLK_PRINTSCREEN, KEY_PRINT);
    // execute missing
    KeysMaps.emplace_back(SDLK_PRINTSCREEN, KEY_SNAPSHOT);

    KeysMaps.emplace_back(SDLK_INSERT, KEY_INSERT);
    KeysMaps.emplace_back(SDLK_DELETE, KEY_DELETE);
    KeysMaps.emplace_back(SDLK_HELP, KEY_HELP);

    KeysMaps.emplace_back(SDLK_0, KEY_KEY_0);
    KeysMaps.emplace_back(SDLK_1, KEY_KEY_1);
    KeysMaps.emplace_back(SDLK_2, KEY_KEY_2);
    KeysMaps.emplace_back(SDLK_3, KEY_KEY_3);
    KeysMaps.emplace_back(SDLK_4, KEY_KEY_4);
    KeysMaps.emplace_back(SDLK_5, KEY_KEY_5);
    KeysMaps.emplace_back(SDLK_6, KEY_KEY_6);
    KeysMaps.emplace_back(SDLK_7, KEY_KEY_7);
    KeysMaps.emplace_back(SDLK_8, KEY_KEY_8);
    KeysMaps.emplace_back(SDLK_9, KEY_KEY_9);

    KeysMaps.emplace_back(SDLK_a, KEY_KEY_A);
    KeysMaps.emplace_back(SDLK_b, KEY_KEY_B);
    KeysMaps.emplace_back(SDLK_c, KEY_KEY_C);
    KeysMaps.emplace_back(SDLK_d, KEY_KEY_D);
    KeysMaps.emplace_back(SDLK_e, KEY_KEY_E);
    KeysMaps.emplace_back(SDLK_f, KEY_KEY_F);
    KeysMaps.emplace_back(SDLK_g, KEY_KEY_G);
    KeysMaps.emplace_back(SDLK_h, KEY_KEY_H);
    KeysMaps.emplace_back(SDLK_i, KEY_KEY_I);
    KeysMaps.emplace_back(SDLK_j, KEY_KEY_J);
    KeysMaps.emplace_back(SDLK_k, KEY_KEY_K);
    KeysMaps.emplace_back(SDLK_l, KEY_KEY_L);
    KeysMaps.emplace_back(SDLK_m, KEY_KEY_M);
    KeysMaps.emplace_back(SDLK_n, KEY_KEY_N);
    KeysMaps.emplace_back(SDLK_o, KEY_KEY_O);
    KeysMaps.emplace_back(SDLK_p, KEY_KEY_P);
    KeysMaps.emplace_back(SDLK_q, KEY_KEY_Q);
    KeysMaps.emplace_back(SDLK_r, KEY_KEY_R);
    KeysMaps.emplace_back(SDLK_s, KEY_KEY_S);
    KeysMaps.emplace_back(SDLK_t, KEY_KEY_T);
    KeysMaps.emplace_back(SDLK_u, KEY_KEY_U);
    KeysMaps.emplace_back(SDLK_v, KEY_KEY_V);
    KeysMaps.emplace_back(SDLK_w, KEY_KEY_W);
    KeysMaps.emplace_back(SDLK_x, KEY_KEY_X);
    KeysMaps.emplace_back(SDLK_y, KEY_KEY_Y);
    KeysMaps.emplace_back(SDLK_z, KEY_KEY_Z);

    KeysMaps.emplace_back(SDLK_LGUI, KEY_LWIN);
    KeysMaps.emplace_back(SDLK_RGUI, KEY_RWIN);
    // apps missing
    KeysMaps.emplace_back(SDLK_POWER, KEY_SLEEP); //??

    KeysMaps.emplace_back(SDLK_KP_0, KEY_NUMPAD0);
    KeysMaps.emplace_back(SDLK_KP_1, KEY_NUMPAD1);
    KeysMaps.emplace_back(SDLK_KP_2, KEY_NUMPAD2);
    KeysMaps.emplace_back(SDLK_KP_3, KEY_NUMPAD3);
    KeysMaps.emplace_back(SDLK_KP_4, KEY_NUMPAD4);
    KeysMaps.emplace_back(SDLK_KP_5, KEY_NUMPAD5);
    KeysMaps.emplace_back(SDLK_KP_6, KEY_NUMPAD6);
    KeysMaps.emplace_back(SDLK_KP_7, KEY_NUMPAD7);
    KeysMaps.emplace_back(SDLK_KP_8, KEY_NUMPAD8);
    KeysMaps.emplace_back(SDLK_KP_9, KEY_NUMPAD9);
    KeysMaps.emplace_back(SDLK_KP_MULTIPLY, KEY_MULTIPLY);
    KeysMaps.emplace_back(SDLK_KP_PLUS, KEY_ADD);
    KeysMaps.emplace_back(SDLK_KP_ENTER, KEY_RETURN);
    KeysMaps.emplace_back(SDLK_KP_MINUS, KEY_SUBTRACT);
    KeysMaps.emplace_back(SDLK_KP_PERIOD, KEY_DECIMAL);
    KeysMaps.emplace_back(SDLK_KP_DIVIDE, KEY_DIVIDE);

    KeysMaps.emplace_back(SDLK_F1, KEY_F1);
    KeysMaps.emplace_back(SDLK_F2, KEY_F2);
    KeysMaps.emplace_back(SDLK_F3, KEY_F3);
    KeysMaps.emplace_back(SDLK_F4, KEY_F4);
    KeysMaps.emplace_back(SDLK_F5, KEY_F5);
    KeysMaps.emplace_back(SDLK_F6, KEY_F6);
    KeysMaps.emplace_back(SDLK_F7, KEY_F7);
    KeysMaps.emplace_back(SDLK_F8, KEY_F8);
    KeysMaps.emplace_back(SDLK_F9, KEY_F9);
    KeysMaps.emplace_back(SDLK_F10, KEY_F10);
    KeysMaps.emplace_back(SDLK_F11, KEY_F11);
    KeysMaps.emplace_back(SDLK_F12, KEY_F12);
    KeysMaps.emplace_back(SDLK_F13, KEY_F13);
    KeysMaps.emplace_back(SDLK_F14, KEY_F14);
    KeysMaps.emplace_back(SDLK_F15, KEY_F15);
    // no higher F-keys

    KeysMaps.emplace_back(SDLK_NUMLOCKCLEAR, KEY_NUMLOCK);
    KeysMaps.emplace_back(SDLK_SCROLLLOCK, KEY_SCROLL);
    KeysMaps.emplace_back(SDLK_LSHIFT, KEY_LSHIFT);
    KeysMaps.emplace_back(SDLK_RSHIFT, KEY_RSHIFT);
    KeysMaps.emplace_back(SDLK_LCTRL, KEY_LCONTROL);
    KeysMaps.emplace_back(SDLK_RCTRL, KEY_RCONTROL);
    KeysMaps.emplace_back(SDLK_LALT, KEY_LMENU);
    KeysMaps.emplace_back(SDLK_RALT, KEY_RMENU);

    KeysMaps.emplace_back(SDLK_PLUS, KEY_PLUS);
    KeysMaps.emplace_back(SDLK_COMMA, KEY_COMMA);
    KeysMaps.emplace_back(SDLK_MINUS, KEY_MINUS);
    KeysMaps.emplace_back(SDLK_PERIOD, KEY_PERIOD);

    // some special keys missing

    std::sort(KeysMaps.begin(), KeysMaps.end());
}

u32 MainWindow::checkSuccessiveClicks(s32 mouseX, s32 mouseY, MouseInputEventType inputEvent)
{
    const s32 MAX_MOUSEMOVE = 3;

    u32 clickTime = TimeCounter::getRealTime();

    if ((clickTime - MultiClicks.LastClickTime) < MultiClicks.DoubleClickTime &&
        std::abs(MultiClicks.LastClick.X - mouseX) <= MAX_MOUSEMOVE &&
        std::abs(MultiClicks.LastClick.Y - mouseY) <= MAX_MOUSEMOVE &&
        MultiClicks.CountSuccessiveClicks < 3 && MultiClicks.LastMouseInputEvent == inputEvent) {
        ++MultiClicks.CountSuccessiveClicks;
    } else {
        MultiClicks.CountSuccessiveClicks = 1;
    }

    MultiClicks.LastMouseInputEvent = inputEvent;
    MultiClicks.LastClickTime = clickTime;
    MultiClicks.LastClick.X = mouseX;
    MultiClicks.LastClick.Y = mouseY;

    return MultiClicks.CountSuccessiveClicks;
}

void MainWindow::resetReceiveTextInputEvents(const recti &textarea, bool acceptIME)
{
    if (acceptIME) {
        // IBus seems to have an issue where dead keys and compose keys do not
        // work (specifically, the individual characters in the sequence are
        // sent as text input events instead of the result) when
        // SDL_StartTextInput() is called on the same input box.
        if (!SDL_IsTextInputActive() || LastTextArea != textarea) {
            LastTextArea = textarea;
            SDL_Rect rect;
            rect.x = textarea.ULC.X;
            rect.y = textarea.ULC.Y;
            rect.w = textarea.getWidth();
            rect.h = textarea.getHeight();
            SDL_SetTextInputRect(&rect);
            SDL_StartTextInput();
        }
    } else {
        SDL_StopTextInput();
    }
}

}
