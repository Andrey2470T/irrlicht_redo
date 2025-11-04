#pragma once

#include "GLVersionSpecific.h"
#include "OpenGLIncludes.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_video.h>
#include <SDL_joystick.h>
#include <queue>
#include "CursorControl.h"
#include "Image/Image.h"
#include "Events.h"

#ifdef _EMSCRIPTEN_
#include <emscripten/html5.h>
#endif

// DirectFB is removed in SDL3, thou distribution as Alpine currently ships SDL2
// with enabled DirectFB, but requiring another fix at a top of SDL2.
// We don't need DirectFB in Irrlicht/Minetest, so simply disable it here to prevent issues.
#undef SDL_VIDEO_DRIVER_DIRECTFB
#include <SDL_syswm.h>

namespace core
{

struct JoystickInfo;

// COSOperator interface
class Clipboard
{
    mutable char *ClipboardSelectionText = nullptr;
    mutable char *PrimarySelectionText = nullptr;
public:
    Clipboard() = default;
    ~Clipboard();

    void copyToClipboard(const c8 *text) const;
    void copyToPrimarySelection(const c8 *text) const;
    const c8 *getTextFromClipboard() const;
    const c8 *getTextFromPrimarySelection() const;
};

struct MainWindowParameters
{
	u32 Width = 800;
	u32 Height = 600;

    v2i Pos{-1, -1};

	std::wstring Caption = L"";

    OpenGLType GLType = OGL_TYPE_DESKTOP;
    std::optional<u8> GLVersionMajor;
    std::optional<u8> GLVersionMinor;

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
    SDL_GLContext Context;
    SDL_Window* Window = nullptr;
#ifdef _COMPILE_WITH_JOYSTICK_EVENTS_
    std::vector<SDL_Joystick *> Joysticks;
#endif

    std::queue<Event> Events;

    SDL_version SDLVersion;
    OpenGLVersion GLVersion;

    std::unique_ptr<GLParameters> GLParams;
    Clipboard SDLClipboard;

    struct KeysMap
    {
        KeysMap() :
            SDLKey(0), Win32Key(0)
        {}
        KeysMap(s32 x11, s32 win32) :
            SDLKey(x11), Win32Key(win32)
        {}

        s32 SDLKey{0};
        s32 Win32Key{0};

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

    v2i MousePos{0, 0};
    v2i MouseRelPos{0, 0};
    u32 MouseButtonStates{0};

    v2f Scale{1.0f, 1.0f};

    s32 CurrentTouchCount{0};
    bool IsInBackground{false};

    recti LastTextArea{0, 0, 0, 0};

	bool Resizable;
    bool Close{false};
public:
    bool shouldUpdateViewport{false};

    MainWindow(const MainWindowParameters &params);

	~MainWindow();

	// Window params adjusting methods
    void setIcon(img::Image *newImg);
    void setCaption(const std::wstring &newCaption);

	bool isActive() const;
	bool isFocused() const;
	bool isVisible() const;
	bool isMinimized() const;
	bool isMaximized() const;
	bool isFullScreen() const;
    bool isClosed() const;

    void close();
	void setResizable(bool resize = false);
	void minimize();
	void maximize();
	void restore();
    bool setFullscreen(bool fullscreen);

	void SwapWindow();

    bool isUsingWayland() const;

    bool activateJoysticks(std::vector<JoystickInfo> &joysticksInfo);
    std::optional<Event> popEvent();
    void clearEventQueue();
    bool pollEventsFromQueue();
    void resetReceiveTextInputEvents(const recti &textarea, bool acceptIME);

    std::string getVendorName() const;
    std::string getGLVersionString() const;

    SDL_Window *getWindow() const
    {
        return Window;
    }
    v2u getWindowSize() const;
    v2u getViewportSize() const;
    u32 getFullscreenFlag(bool fullscreen);
    f32 getDisplayDensity() const;
    u32 getScreenDPI() const;
    const OpenGLVersion &getOpenGLVersion() const;
    const GLParameters *getGLParams() const;
    const Clipboard *getClipboard() const;
    CursorControl &getCursorControl();
private:
#ifdef _EMSCRIPTEN_
	static EM_BOOL MouseUpDownCallback(int eventType, const EmscriptenMouseEvent *event, void *userData);
	static EM_BOOL MouseEnterCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
	static EM_BOOL MouseLeaveCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
#endif
    SDL_GLprofile convertIrrGLTypeToProfile() const;

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


