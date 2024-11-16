#include "MainWindow.h"

#ifdef _IRR_EMSCRIPTEN_PLATFORM_
#include <emscripten.h>
#endif

MainWindow::MainWindow(const MainWindowParameters &params)
	: GLVersion(Params.GLType), Timer(false), CursorControl(this), Params(params),
	  Resizable(params.Resizable == 1 ? true : false), Close(false)
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
		ERR_LOG_WITH_HINT("Unable to initialize SDL", SDL_GetError());
		Close = true;
	} else {
		LOG("SDL initialized");
	}

	if (params.InitVideo)
		if (!initWindow()) {
			Close = true;
			ERR_LOG("Unable to initialize SDL window and context (InitVideo = true)");
		}

	SDL_GetWindowWMInfo(&Window, &Info);

	LOG("SDL Version " + Info.version.major + "." + Info.version.minor + "." + Info.version.patch);
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
	SDL_GetWindowSize(Window.get(), &window_w, &window_h);

	int drawable_w, drawable_h;
	SDL_GL_GetDrawableSize(Window.get(), &drawable_w, &drawable_h);

	ScaleX = (float)drawable_w / (float)window_w;
	ScaleY = (float)drawable_h / (float)window_h;
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

#ifdef _IRR_EMSCRIPTEN_PLATFORM_
	if (Params.Width != 0 || Params.Height != 0)
		emscripten_set_canvas_size(Params.Width, Params.Height);
	else {
		int fs;
		emscripten_get_canvas_size(&Params.Width, &Params.Height, &fs);
	}
#endif

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

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GLVersion.Major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GLVersion.Minor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, GLVersion.Profile);

	if (Params.DriverDebug) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG);
	}

#ifdef _IRR_EMSCRIPTEN_PLATFORM_
	SDL_Window *wnd;
	SDL_Renderer *rndr;
	SDL_CreateWindowAndRenderer(0, 0, Wnd_Flags, &wnd, &rndr); // 0,0 will use the canvas size
	Window = std::unique_ptr<SDL_Window>(wnd);
	Renderer = std::unique_ptr<SDL_Renderer>(rndr);

	// "#canvas" is for the opengl context
	emscripten_set_mousedown_callback("#canvas", (void *)this, true, MouseUpDownCallback);
	emscripten_set_mouseup_callback("#canvas", (void *)this, true, MouseUpDownCallback);
	emscripten_set_mouseenter_callback("#canvas", (void *)this, false, MouseEnterCallback);
	emscripten_set_mouseleave_callback("#canvas", (void *)this, false, MouseLeaveCallback);

	return true;
#else // !_IRR_EMSCRIPTEN_PLATFORM_
	Window = std::unique_ptr<SDL_Window>(
		SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Params.Width, Params.Height, Wnd_Flags));

	if (!Window) {
		ERR_LOG_WITH_HINT("Could not create window", SDL_GetError());
		return false;
	}

	Context = std::unique_ptr<SDL_GLContext>(SDL_GL_CreateContext(Window));
	if (!Context) {
		ERR_LOG_WITH_HINT("Could not create context", SDL_GetError());
		return false;
	}

	updateSizeAndScale();
	if (ScaleX != 1.0f || ScaleY != 1.0f) {
		// The given window size is in pixels, not in screen coordinates.
		// We can only do the conversion now since we didn't know the scale before.
		SDL_SetWindowSize(Window,
				static_cast<int>(CreationParams.WindowSize.Width / ScaleX),
				static_cast<int>(CreationParams.WindowSize.Height / ScaleY));
		// Re-center, otherwise large, non-maximized windows go offscreen.
		SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		updateSizeAndScale();
	}

	return true;
#endif // !_IRR_EMSCRIPTEN_PLATFORM_
}
