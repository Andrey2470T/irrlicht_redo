#include "MainWindow.h"
#include "Image/Converting.h"
#include "Utils/String.h"

#ifdef _IRR_EMSCRIPTEN_PLATFORM_
#include <emscripten.h>
#endif


MainWindow::MainWindow(const MainWindowParameters &params)
	: GLVersion(Params.GLType), Timer(false), CursorControl(this), Params(std::move(params)),
	  Resizable(Params.Resizable == 1 ? true : false), Close(false), IsInBackground(false)
{
#ifdef __ANDROID__
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

#if defined(_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
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

#if defined(SDL_HINT_APP_NAME)
	SDL_SetHint(SDL_HINT_APP_NAME, "Minetest");
#endif

	// Set IME hints
	SDL_SetHint(SDL_HINT_IME_INTERNAL_EDITING, "1");
#if defined(SDL_HINT_IME_SHOW_UI)
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

	u32 flags = SDL_INIT_TIMER | SDL_INIT_EVENTS;
	if (Params.InitVideo)
		flags |= SDL_INIT_VIDEO;
#if defined(_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
	flags |= SDL_INIT_JOYSTICK;
#endif

	if (SDL_Init(flags) < 0) {
		SDL_LogError(LC_APP, "Unable to initialize SDL: " + SDL_GetError());
		Close = true;
	} else {
		SDL_LogInfo(LC_APP, "SDL initialized");
	}

	if (Params.InitVideo)
		if (!initWindow()) {
			Close = true;
			SDL_LogError(LC_APP, "Unable to initialize SDL window and context (InitVideo = true)");
		}

	SDL_VERSION(&SDLVersion);

	SDL_LogInfo(LC_APP, "SDL Version: " + SDLVersion.major + "." + SDLVersion.minor + "." + SDLVersion.patch);
}

MainWindow::~MainWindow()
{

	if (Window)
		SDL_DestroyWindow(Window);
	
#ifdef _IRR_EMSCRIPTEN_PLATFORM_
	if (Renderer)
		SDL_DestroyRenderer(Renderer);
#else
	if (Context)
		SDL_GL_DeleteContext(Context);
#endif
	
#ifdef _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
	for (auto &joystick : Joysticks)
		if (joystick)
			SDL_JoystickClose(joystick);
#endif
	SDL_LogInfo(LC_APP, "Quit SDL");
}


// Window params adjusting methods
void MainWindow::setIcon(std::shared_ptr<Image> newImg, img::ImageModifier *mdf)
{
	if (!Window)
		return false;

	u32 height = newImg->getHeight();
	u32 width = newImg->getWidth();

	SDL_Surface *surface = SDL_CreateRGBSurface(0, width, height, 32,
			0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	if (!surface) {
		SDL_LogError(LC_APP, "Failed to create SDL suface");
		return false;
	}

	SDL_LockSurface(surface);
	
	img::Image *tempImg = convertSDLSurfaceToImage(surface);
	bool succ = mdf->copyTo(newImg.get(), tempImg);
	delete tempImg;

	SDL_UnlockSurface(surface);

	if (!succ) {
		SDL_FreeSurface(surface);
		return false;
	}

	SDL_SetWindowIcon(Window, surface);

	SDL_FreeSurface(surface);

	return true;
}

void MainWindow::setCaption(const std::wstring &newCaption)
{
	std::string utf8_str = wide_to_utf8(newCaption);
	SDL_SetWindowTitle(Window, utf8_str.c_str());
}

bool MainWindow::isActive() const
{
#ifdef _IRR_EMSCRIPTEN_PLATFORM_
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

void MainWindow::setResizable(bool resize = false)
{
#ifdef _IRR_EMSCRIPTEN_PLATFORM_
	SDL_LogWarn("Resizable not available on the web.");
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
	if (!success)
		SDL_LogError("SDL_SetWindowFullscreen failed: " + SDL_GetError());
	return success;
}

void MainWindow::SwapWindow()
{
	if (Window)
		SDL_GL_SwapWindow(Window);
}

u32 MainWindow::getFullScreenFlag(bool fullscreen)
{
	if (!fullscreen)
		return 0;
#ifdef _IRR_EMSCRIPTEN_PLATFORM_
	return SDL_WINDOW_FULLSCREEN;
#else
	return SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif
}

void MainWindow::updateViewportAndScale()
{
	int window_w, window_h;
	SDL_GetWindowSize(Window, &window_w, &window_h);

	SDL_GL_GetDrawableSize(Window, &Params.Width, &Params.Height);

	Scale = utils::v2f(
		(float)Params.Width / (float)window_w,
		(float)Params.Height / (float)window_h);
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
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, CreationParams.SwapBuffers ? 1 : 0);

	if (Params.AntiAlias > 1) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, Params.AntiAlias);
	} else {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
	}

#ifdef _IRR_EMSCRIPTEN_PLATFORM_
	if (Params.Width != 0 || Params.Height != 0)
		emscripten_set_canvas_size(Params.Width, Params.Height);
	else
		emscripten_get_canvas_size(&Params.Width, &Params.Height, nullptr);

	SDL_CreateWindowAndRenderer(0, 0, Wnd_Flags, &Window, &Renderer); // 0,0 will use the canvas size
	
	if (!Window || !Renderer) {
		SDL_LogError(LC_APP, "Could not create window or renderer: " + SDL_GetError());
		Close = true;
		return false;
	}

	// "#canvas" is for the opengl context
	emscripten_set_mousedown_callback("#canvas", (void *)this, true, MouseUpDownCallback);
	emscripten_set_mouseup_callback("#canvas", (void *)this, true, MouseUpDownCallback);
	emscripten_set_mouseenter_callback("#canvas", (void *)this, false, MouseEnterCallback);
	emscripten_set_mouseleave_callback("#canvas", (void *)this, false, MouseLeaveCallback);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GLVersion.Major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GLVersion.Minor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, GLVersion.Profile);

	if (Params.DriverDebug)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG);

	Window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Params.Width, Params.Height, Wnd_Flags);

	if (!Window) {
		SDL_LogError(LC_APP, "Could not create window: " + SDL_GetError());
		Close = true;
		return false;
	}

	Context = SDL_GL_CreateContext(Window);
	if (!Context) {
		SDL_LogError(LC_APP, "Could not create context: " + SDL_GetError());
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

	return true;
#endif
}
