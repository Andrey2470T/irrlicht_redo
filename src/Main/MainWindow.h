#pragma once

#include "BasicIncludes.h"
#include "CursorControl.h"
#include "TimeCounter.h"

#include <SDL.h>

#ifdef _IRR_EMSCRIPTEN_PLATFORM_
#include <emscripten/html5.h>
#endif

// DirectFB is removed in SDL3, thou distribution as Alpine currently ships SDL2
// with enabled DirectFB, but requiring another fix at a top of SDL2.
// We don't need DirectFB in Irrlicht/Minetest, so simply disable it here to prevent issues.
#undef SDL_VIDEO_DRIVER_DIRECTFB
#include <SDL_syswm.h>

namespace main
{

enum OpenGLType
{
	OGL_TYPE_DESKTOP,
	OGL_TYPE_ES,
	OGL_TYPE_WEB
};

struct OpenGLVersion
{
	SDL_GLprofile Profile;

	u8 Major;
	u8 Minor;
public:
	OpenGLVersion(OpenGLType type)
	{
		switch (type) {
		case OGL_TYPE_DESKTOP:
			Profile = SDL_GL_CONTEXT_PROFILE_CORE;
			Major = 3;
			Minor = 2;
		case OGL_TYPE_ES:
		case OGL_TYPE_WEB:
			Profile = SDL_GL_CONTEXT_PROFILE_ES;
			Major = 2;
			Minor = 0;
		}
	}
};


struct MainWindowParameters
{
	u32 Width = 800;
	u32 Height = 600;

	utils::v2i Pos = utils::v2i(-1, -1);

	std::wstring Caption = L"";

	OpenGLType GLType = OGL_TYPE_DESKTOP;

	u8 ColorChannelBits = 8;
	u8 DepthBits = 24;
	u8 StencilBits = 8;

	bool FullScreen = false;
	bool Maximized = false;
	u8 Resizable = 2;
	bool VSync = false;
	bool StereoBuffer = true;
	bool SwapBuffers = true;
	bool InitVideo = true;

	u8 AntiAlias = 0;
};

class MainWindow
{
	SDL_Window* Window;
#ifdef _IRR_EMSCRIPTEN_PLATFORM_
	SDL_Renderer* Renderer;
#else
	SDL_GLContext* Context;
#endif
#ifdef _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
	std::vector<SDL_Joystick*> Joysticks;
#endif
	SDL_version SDLVersion;
	OpenGLVersion GLVersion;

	TimeCounter Timer;
	CursorControl Cursor;

	MainWindowParameters Params;

	utils::v2i MousePos;
	utils::v2i MouseRelPos;
	u32 MouseButtonStates;

	utils::v2f Scale{1.0f, 1.0f};
	f32 DisplayDensity;

	s32 CurrentTouchCount;
	bool IsInBackground;

	bool Resizable;
	bool Close;
public:
	MainWindow(const MainWindowParameters &params);

	~MainWindow();

	// Window params adjusting methods
	void setIcon(std::shared_ptr<Image> newImg);
	void setCaption(const std::wstring &newCaption);

	// Window time counter
	void startTimer();
	void stopTimer();

	bool isActive() const;
	bool isFocused() const;
	bool isVisible() const;
	bool isMinimized() const;
	bool isMaximized() const;
	bool isFullScreen() const;

	void setResizable(bool resize = false);
	void minimize();
	void maximize();
	void restore();
	bool setFullscreen();

	void SwapWindow();
private:
#ifdef _IRR_EMSCRIPTEN_PLATFORM_
	static EM_BOOL MouseUpDownCallback(int eventType, const EmscriptenMouseEvent *event, void *userData);
	static EM_BOOL MouseEnterCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
	static EM_BOOL MouseLeaveCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
#endif

	u32 getFullScreenFlag(bool fullscreen);
	void updateViewportAndScale();

	void initWindow();
	
	friend class CursorControl;
};

}


