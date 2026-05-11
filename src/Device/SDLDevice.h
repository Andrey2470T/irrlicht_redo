// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// This device code is based on the original SDL device implementation
// contributed by Shane Parker (sirshane).

#pragma once

#include "Device/ISDLDevice.h"
#include "Device/IEventReceiver.h"
#include "CursorControl.h"
#include "Device/Timer.h"
#include "Device/Clipboard.h"

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


//! Stub for an Irrlicht Device implementation (now merged into SDLDevice)
class SDLDevice : public ISDLDevice
{
public:
	//! constructor
	SDLDevice(const SDLDeviceParameters &param);

	//! destructor
	~SDLDevice();

	//! runs the device. Returns false if device wants to be deleted
	bool run() override;

	//! pause execution temporarily
	void yield() override;

	//! pause execution for a specified time
	void sleep(u32 timeMs, bool pauseTimer=false) override;

	//! sets the caption of the window
	void setWindowCaption(const wchar_t *text) override;

	//! Sets the window icon.
	bool setWindowIcon(const video::Image *img) override;

	//! returns if window is active. if not, nothing need to be drawn
	bool isWindowActive() const override;

	//! returns if window has focus.
	bool isWindowFocused() const override;

	//! returns if window is minimized.
	bool isWindowMinimized() const override;

	//! notifies the device that it should close itself
	void closeDevice() override;

	//! Sets if the window should be resizable in windowed mode.
	void setResizable(bool resize = false) override;

	//! Minimizes the window.
	void minimizeWindow() override;

	//! Maximizes the window.
	void maximizeWindow() override;

	//! Restores the window size.
	void restoreWindow() override;

	//! Checks if the window is maximized.
	bool isWindowMaximized() const override;

	//! Checks if the Irrlicht window is running in fullscreen mode
	/** \return True if window is fullscreen. */
	bool isFullscreen() const override;

	//! Enables or disables fullscreen mode.
	/** \return True on success. */
	bool setFullscreen(bool fullscreen) override;

	//! Checks if the window could possibly be visible.
	bool isWindowVisible() const override;

	//! Checks if the Irrlicht device supports touch events.
	bool supportsTouchEvents() const override;

	//! Get the position of this window on screen
	core::position2di getWindowPosition() override;

	//! Activate any joysticks, and generate events for them.
	bool activateJoysticks(core::array<SJoystickInfo> &joystickInfo) override;

	//! Get the SDL version
	std::string getVersionString() const override;

	//! Get the display density in dots per inch.
	float getDisplayDensity() const override;

	//! Activate accelerometer.
	bool activateAccelerometer(float updateInterval = 0.016666f) override;

	//! Deactivate accelerometer.
	bool deactivateAccelerometer() override;

	//! Is accelerometer active.
	bool isAccelerometerActive() override;

	//! Is accelerometer available.
	bool isAccelerometerAvailable() override;

	//! Activate gyroscope.
	bool activateGyroscope(float updateInterval = 0.016666f) override;

	//! Deactivate gyroscope.
	bool deactivateGyroscope() override;

	//! Is gyroscope active.
	bool isGyroscopeActive() override;

	//! Is gyroscope available.
	bool isGyroscopeAvailable() override;

	//! Activate device motion.
	bool activateDeviceMotion(float updateInterval = 0.016666f) override;

	//! Deactivate device motion.
	bool deactivateDeviceMotion() override;

	//! Is device motion active.
	bool isDeviceMotionActive() override;

	//! Is device motion available.
	bool isDeviceMotionAvailable() override;

	//! Set the maximal elapsed time between 2 clicks to generate doubleclicks for the mouse. It also affects tripleclick behavior.
	void setDoubleClickTime(u32 timeMs) override;

	//! Get the maximal elapsed time between 2 clicks to generate double- and tripleclicks for the mouse.
	u32 getDoubleClickTime() const override;

	//! Remove all messages pending in the system message loop
	void clearSystemMessages() override;

	//! Resize the render window.
	void setWindowSize(const core::dimension2d<u32> &size) override {}

	bool swapBuffers() override;

	//! returns the video driver
	video::VideoDriver *getVideoDriver() override;

	//! return file system
	io::IFileSystem *getFileSystem() override;

	//! returns the gui environment
	gui::IGUIEnvironment *getGUIEnvironment() override;

	//! returns the scene manager
	scene::ISceneManager *getSceneManager() override;

	//! \return Returns a pointer to the mouse cursor control interface.
	gui::CursorControl *getCursorControl() override;

	//! send the event to the right receiver
	bool postEventFromUser(const SEvent &event) override;

	//! Sets a new event receiver to receive events
	void setEventReceiver(IEventReceiver *receiver) override;

	//! Returns pointer to the current event receiver. Returns 0 if there is none.
	IEventReceiver *getEventReceiver() override;

	//! Sets the input receiving scene manager.
	/** If set to null, the main scene manager (returned by GetSceneManager()) will receive the input */
	void setInputReceivingSceneManager(scene::ISceneManager *sceneManager) override;

	//! Returns a pointer to the logger.
	os::Logger *getLogger() override;

	//! Returns the operation system opertator object.
	os::Clipboard *getOSOperator() override;

	std::variant<u32, EKEY_CODE> getScancodeFromKey(const Keycode &key) const override;
	Keycode getKeyFromScancode(const u32 scancode) const override;

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
