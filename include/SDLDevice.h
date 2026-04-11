// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// This device code is based on the original SDL device implementation
// contributed by Shane Parker (sirshane).

#pragma once

#include "IReferenceCounted.h"
#include "dimension2d.h"
#include "EDriverTypes.h"
#include "IEventReceiver.h"
#include "CursorControl.h"
#include "Timer.h"
#include "Clipboard.h"
#include "irrArray.h"
#include "position2d.h"
#include "SColor.h" // video::ECOLOR_FORMAT
#include <string>
#include <variant>
#include "SDLDeviceParameters.h"

#ifdef _IRR_EMSCRIPTEN_PLATFORM_
#include <emscripten/html5.h>
#endif

#ifdef _IRR_USE_SDL3_
#define SDL_DISABLE_OLD_NAMES
#include <SDL3/SDL.h>
#include <SDL3/SDL_mouse.h>
#else
#include <SDL.h>
#endif

#include <memory>
#include <unordered_map>

namespace os
{
class Logger;
}

namespace video
{
class IImage;
class GLTexture;
class VideoDriver;
}

namespace io
{
class IFileSystem;
IFileSystem *createFileSystem();
}

namespace gui
{
class IGUIEnvironment;
IGUIEnvironment *createGUIEnvironment(io::IFileSystem *fs,
	video::VideoDriver *Driver, os::Clipboard *op);
}

namespace scene
{
class ISceneManager;
ISceneManager *createSceneManager(video::VideoDriver *driver, gui::CursorControl *cc);
}

class IEventReceiver;

//! Stub for an Irrlicht Device implementation (now merged into SDLDevice)
class SDLDevice : public virtual IReferenceCounted
{
public:
	//! Creates an Irrlicht device. The Irrlicht device is the root object for using the engine.
	/** If you need more parameters to be passed to the creation of the Irrlicht Engine device,
	use the createDeviceEx() function.
	\param driverType: Type of the video driver to use.
	\param windowSize: Size of the window or the video mode in fullscreen mode.
	\param bits: Bits per pixel in fullscreen mode. Ignored if windowed mode.
	\param fullscreen: Should be set to true if the device should run in fullscreen. Otherwise
		the device runs in windowed mode.
	\param stencilbuffer: Specifies if the stencil buffer should be enabled. Set this to true,
	if you want the engine be able to draw stencil buffer shadows. Note that not all
	devices are able to use the stencil buffer. If they don't no shadows will be drawn.
	\param vsync: Specifies vertical synchronization: If set to true, the driver will wait
	for the vertical retrace period, otherwise not.
	\param receiver: A user created event receiver.
	\return Returns pointer to the created IrrlichtDevice or null if the
	device could not be created.
	*/
	static SDLDevice *createDevice(
		video::E_DRIVER_TYPE driverType = video::EDT_OPENGL3,
		// parentheses are necessary for some compilers
		const core::dimension2d<u32> &windowSize = (core::dimension2d<u32>(640, 480)),
		u32 bits = 32,
		bool fullscreen = false,
		bool stencilbuffer = true,
		bool vsync = false,
		IEventReceiver *receiver = 0);

	//! Creates an Irrlicht device with the option to specify advanced parameters.
	/** Usually you should used createDevice() for creating an Irrlicht Engine device.
	Use this function only if you wish to specify advanced parameters like a window
	handle in which the device should be created.
	\param parameters: Structure containing advanced parameters for the creation of the device.
	See SDLDeviceParameters for details.
	\return Returns pointer to the created IrrlichtDevice or null if the
	device could not be created. */
	static SDLDevice *createDeviceEx(
		const SDLDeviceParameters &parameters);

	//! constructor
	SDLDevice(const SDLDeviceParameters &param);

	//! destructor
	~SDLDevice();

	//! runs the device. Returns false if device wants to be deleted
	bool run();

	//! pause execution temporarily
	void yield();

	//! pause execution for a specified time
	void sleep(u32 timeMs, bool pauseTimer=false);

	//! sets the caption of the window
	void setWindowCaption(const wchar_t *text);

	//! Sets the window icon.
	bool setWindowIcon(const video::IImage *img);

	//! returns if window is active. if not, nothing need to be drawn
	bool isWindowActive() const;

	//! returns if window has focus.
	bool isWindowFocused() const;

	//! returns if window is minimized.
	bool isWindowMinimized() const;

	//! notifies the device that it should close itself
	void closeDevice();

	//! Sets if the window should be resizable in windowed mode.
	void setResizable(bool resize = false);

	//! Minimizes the window.
	void minimizeWindow();

	//! Maximizes the window.
	void maximizeWindow();

	//! Restores the window size.
	void restoreWindow();

	//! Checks if the window is maximized.
	bool isWindowMaximized() const;

	//! Checks if the Irrlicht window is running in fullscreen mode
	/** \return True if window is fullscreen. */
	bool isFullscreen() const;

	//! Enables or disables fullscreen mode.
	/** \return True on success. */
	bool setFullscreen(bool fullscreen);

	//! Checks if the window could possibly be visible.
	bool isWindowVisible() const;

	//! Checks if the Irrlicht device supports touch events.
	bool supportsTouchEvents() const;

	//! Get the position of this window on screen
	core::position2di getWindowPosition();

	//! Activate any joysticks, and generate events for them.
	bool activateJoysticks(core::array<SJoystickInfo> &joystickInfo);

	//! Get the SDL version
	std::string getVersionString() const;

	//! Get the display density in dots per inch.
	float getDisplayDensity() const;

	//! Activate accelerometer.
	bool activateAccelerometer(float updateInterval = 0.016666f);

	//! Deactivate accelerometer.
	bool deactivateAccelerometer();

	//! Is accelerometer active.
	bool isAccelerometerActive();

	//! Is accelerometer available.
	bool isAccelerometerAvailable();

	//! Activate gyroscope.
	bool activateGyroscope(float updateInterval = 0.016666f);

	//! Deactivate gyroscope.
	bool deactivateGyroscope();

	//! Is gyroscope active.
	bool isGyroscopeActive();

	//! Is gyroscope available.
	bool isGyroscopeAvailable();

	//! Activate device motion.
	bool activateDeviceMotion(float updateInterval = 0.016666f);

	//! Deactivate device motion.
	bool deactivateDeviceMotion();

	//! Is device motion active.
	bool isDeviceMotionActive();

	//! Is device motion available.
	bool isDeviceMotionAvailable();

	//! Set the maximal elapsed time between 2 clicks to generate doubleclicks for the mouse. It also affects tripleclick behavior.
	void setDoubleClickTime(u32 timeMs);

	//! Get the maximal elapsed time between 2 clicks to generate double- and tripleclicks for the mouse.
	u32 getDoubleClickTime() const;

	//! Remove all messages pending in the system message loop
	void clearSystemMessages();

	//! Resize the render window.
	void setWindowSize(const core::dimension2d<u32> &size) {}

	bool swapBuffers();

	//! returns the video driver
	video::VideoDriver *getVideoDriver();

	//! return file system
	io::IFileSystem *getFileSystem();

	//! returns the gui environment
	gui::IGUIEnvironment *getGUIEnvironment();

	//! returns the scene manager
	scene::ISceneManager *getSceneManager();

	//! \return Returns a pointer to the mouse cursor control interface.
	gui::CursorControl *getCursorControl();

	//! send the event to the right receiver
	bool postEventFromUser(const SEvent &event);

	//! Sets a new event receiver to receive events
	void setEventReceiver(IEventReceiver *receiver);

	//! Returns pointer to the current event receiver. Returns 0 if there is none.
	IEventReceiver *getEventReceiver();

	//! Sets the input receiving scene manager.
	/** If set to null, the main scene manager (returned by GetSceneManager()) will receive the input */
	void setInputReceivingSceneManager(scene::ISceneManager *sceneManager);

	//! Returns a pointer to the logger.
	os::Logger *getLogger();

	//! Returns the operation system opertator object.
	os::Clipboard *getOSOperator();

	std::variant<u32, EKEY_CODE> getScancodeFromKey(const Keycode &key) const;
	Keycode getKeyFromScancode(const u32 scancode) const;

private:
	//! Compares to the last call of this function to return double and triple clicks.
	/** Needed for win32 device event handling
	\return Returns only 1,2 or 3. A 4th click will start with 1 again.
	*/
	u32 checkSuccessiveClicks(s32 mouseX, s32 mouseY, EMOUSE_INPUT_EVENT inputEvent);

	//! Checks whether the input device should take input from the IME
	bool acceptsIME();

#ifdef _IRR_EMSCRIPTEN_PLATFORM_
	static EM_BOOL MouseUpDownCallback(int eventType, const EmscriptenMouseEvent *event, void *userData);
	static EM_BOOL MouseEnterCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
	static EM_BOOL MouseLeaveCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);

#endif
	// Check if a key is a known special character with no side effects on text boxes.
	static bool keyIsKnownSpecial(EKEY_CODE irrlichtKey);

	// Return the Char that should be sent to Irrlicht for the given key (either the one passed in or 0).
	static wchar_t findCharToPassToIrrlicht(uint32_t sdlKey, EKEY_CODE irrlichtKey, u16 keymod);

	// Check if a text box is in focus. Enable or disable SDL_TEXTINPUT events only if in focus.
	void resetReceiveTextInputEvents();

	//! create the driver
	void createDriver();

	//! create GUI and Scene
	void createGUIAndScene();

	bool createWindow();
	bool createWindowWithContext();

	void createKeyMap();

	void logAttributes();

	// video driver
	video::VideoDriver *VideoDrv;
	// gui environment
	gui::IGUIEnvironment *GUIEnvironment;
	// scene manager
	scene::ISceneManager *SceneManager;
	// cursor control
	gui::CursorControl *CursorCtrl;
	// event receiver
	IEventReceiver *UserReceiver;
	// logger
	os::Logger *Logger;
	// clipboard
	os::Clipboard *ClipBoard;
	// file system
	io::IFileSystem *FileSystem;
	// input receiving scene manager
	scene::ISceneManager *InputReceivingSceneManager;
	// creation parameters
	SDLDeviceParameters CreationParams;
	// close flag
	bool Close;

	SDL_GLContext Context;
	SDL_Window *Window;
#if defined(_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
	core::array<SDL_Joystick *> Joysticks;
#endif

	s32 MouseX, MouseY;
	// these two only continue to exist for some Emscripten stuff idk about
	s32 MouseXRel, MouseYRel;
	u32 MouseButtonStates;

	u32 Width, Height;
	f32 ScaleX = 1.0f, ScaleY = 1.0f;
	void updateSizeAndScale();

	bool Resizable;

#ifndef _IRR_USE_SDL3_
	static u32 getFullscreenFlag(bool fullscreen);
#endif // SDL3: Replaced by boolean

	core::rect<s32> lastElemPos;

	// TODO: This is only used for scancode/keycode conversion with EKEY_CODE (among other things, for Luanti
	// to display keys to users). Drop this along with EKEY_CODE.
	std::unordered_map<SDL_Keycode, EKEY_CODE> KeyMap;

	s32 CurrentTouchCount;
	bool IsInBackground;

	struct SMouseMultiClicks
	{
		SMouseMultiClicks() :
				DoubleClickTime(500), CountSuccessiveClicks(0), LastClickTime(0), LastMouseInputEvent(EMIE_COUNT)
		{
		}

		u32 DoubleClickTime;
		u32 CountSuccessiveClicks;
		u32 LastClickTime;
		core::position2di LastClick;
		EMOUSE_INPUT_EVENT LastMouseInputEvent;
	};
	SMouseMultiClicks MouseMultiClicks;

	friend class gui::CursorControl;
};
