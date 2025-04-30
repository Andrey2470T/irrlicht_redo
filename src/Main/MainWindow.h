#pragma once

#include "GLVersionSpecific.h"
#include "CursorControl.h"
#include "Image/Image.h"
#include "Events.h"
#include <queue>

#include <SDL.h>

#ifdef EMSCRIPTEN
#include <emscripten/html5.h>
#endif

// DirectFB is removed in SDL3, thou distribution as Alpine currently ships SDL2
// with enabled DirectFB, but requiring another fix at a top of SDL2.
// We don't need DirectFB in Irrlicht/Minetest, so simply disable it here to prevent issues.
#undef SDL_VIDEO_DRIVER_DIRECTFB
#include <SDL_syswm.h>

namespace main
{

struct JoystickInfo;

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

    bool DriverDebug = false;
};

class MainWindow
{
	SDL_Window* Window;
#ifdef EMSCRIPTEN
	SDL_Renderer* Renderer;
#else
    SDL_GLContext Context;
#endif
#ifdef COMPILE_WITH_JOYSTICK_EVENTS
	std::vector<SDL_Joystick*> Joysticks;
#endif
    std::queue<std::unique_ptr<Event>> Events;

	SDL_version SDLVersion;
    OpenGLVersion GLVersion;

    struct KeysMap
    {
        KeysMap() {}
        KeysMap(s32 x11, s32 win32) :
            SDLKey(x11), Win32Key(win32)
        {
        }

        s32 SDLKey;
        s32 Win32Key;

        bool operator<(const KeysMap &o) const
        {
            return SDLKey < o.SDLKey;
        }
    };
    std::vector<KeysMap> KeysMaps;

    struct MouseMultiClicks
    {
        MouseMultiClicks() :
            DoubleClickTime(500), CountSuccessiveClicks(0), LastClickTime(0), LastMouseInputEvent(MIE_COUNT)
        {
        }

        u32 DoubleClickTime;
        u32 CountSuccessiveClicks;
        u32 LastClickTime;
        v2i LastClick;
        MouseInputEventType LastMouseInputEvent;
    };
    MouseMultiClicks MultiClicks;

	CursorControl Cursor;

	MainWindowParameters Params;

	utils::v2i MousePos;
	utils::v2i MouseRelPos;
	u32 MouseButtonStates;

	utils::v2f Scale{1.0f, 1.0f};
	f32 DisplayDensity;

	s32 CurrentTouchCount;
	bool IsInBackground;

    recti LastTextArea;

	bool Resizable;
	bool Close;
public:
	MainWindow(const MainWindowParameters &params);

	~MainWindow();

	// Window params adjusting methods
    void setIcon(std::shared_ptr<img::Image> newImg, img::ImageModifier *mdf);
    void setCaption(const std::wstring &newCaption);

	bool isActive() const;
	bool isFocused() const;
	bool isVisible() const;
	bool isMinimized() const;
	bool isMaximized() const;
	bool isFullScreen() const;
    bool isClosed() const;

	void setResizable(bool resize = false);
	void minimize();
	void maximize();
	void restore();
    bool setFullscreen(bool fullscreen);

	void SwapWindow();

    bool isUsingWayland() const;

    bool activateJoysticks(std::vector<JoystickInfo> &joysticksInfo);
    bool pollEventsFromQueue();
    void resetReceiveTextInputEvents(const recti &textarea, bool acceptIME);
private:
#ifdef EMSCRIPTEN
	static EM_BOOL MouseUpDownCallback(int eventType, const EmscriptenMouseEvent *event, void *userData);
	static EM_BOOL MouseEnterCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
	static EM_BOOL MouseLeaveCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
#endif

    v2u getWindowSize() const;
    v2u getViewportSize() const;
    u32 getFullscreenFlag(bool fullscreen);
	void updateViewportAndScale();

    bool initWindow();
    void createKeysMaps();

    u32 checkSuccessiveClicks(s32 mouseX, s32 mouseY, MouseInputEventType inputEvent);
	
	friend class CursorControl;
};

struct MouseMultiClicks
{
    MouseMultiClicks() :
        DoubleClickTime(500), CountSuccessiveClicks(0), LastClickTime(0), LastMouseInputEvent(MIE_COUNT)
    {
    }

    u32 DoubleClickTime;
    u32 CountSuccessiveClicks;
    u32 LastClickTime;
    v2i LastClick;
    MouseInputEventType LastMouseInputEvent;
};

}


