// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// This device code is based on the original SDL device implementation
// contributed by Shane Parker (sirshane).

#pragma once

#include "Utils/IReferenceCounted.h"
#include "Utils/dimension2d.h"
#include "Enums/EDriverTypes.h"
#include "Utils/irrArray.h"
#include "Utils/position2d.h"
#include "Image/SColor.h" // video::ECOLOR_FORMAT
#include "IEventReceiver.h"
#include "SDLDeviceParameters.h"
#include <string>
#include <variant>

namespace os
{
class Logger;
class Clipboard;
}

namespace video
{
class Image;
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
class ICursorControl;
IGUIEnvironment *createGUIEnvironment(io::IFileSystem *fs,
	video::VideoDriver *Driver, os::Clipboard *op);
}

namespace scene
{
class ISceneManager;
ISceneManager *createSceneManager(video::VideoDriver *driver, gui::ICursorControl *cc);
}

//! Stub for an Irrlicht Device implementation (now merged into SDLDevice)
class ISDLDevice : public virtual IReferenceCounted
{
public:
	//! constructor
	ISDLDevice() = default;

	//! destructor
	virtual ~ISDLDevice() = default;

	//! runs the device. Returns false if device wants to be deleted
	virtual bool run() = 0;

	//! pause execution temporarily
	virtual void yield() = 0;

	//! pause execution for a specified time
	virtual void sleep(u32 timeMs, bool pauseTimer=false) = 0;

	//! sets the caption of the window
	virtual void setWindowCaption(const wchar_t *text) = 0;

	//! Sets the window icon.
	virtual bool setWindowIcon(const video::Image *img) = 0;

	//! returns if window is active. if not, nothing need to be drawn
	virtual bool isWindowActive() const = 0;

	//! returns if window has focus.
	virtual bool isWindowFocused() const = 0;

	//! returns if window is minimized.
	virtual bool isWindowMinimized() const = 0;

	//! notifies the device that it should close itself
	virtual void closeDevice() = 0;

	//! Sets if the window should be resizable in windowed mode.
	virtual void setResizable(bool resize = false) = 0;

	//! Minimizes the window.
	virtual void minimizeWindow() = 0;

	//! Maximizes the window.
	virtual void maximizeWindow() = 0;

	//! Restores the window size.
	virtual void restoreWindow() = 0;

	//! Checks if the window is maximized.
	virtual bool isWindowMaximized() const = 0;

	//! Checks if the Irrlicht window is running in fullscreen mode
	/** \return True if window is fullscreen. */
	virtual bool isFullscreen() const = 0;

	//! Enables or disables fullscreen mode.
	/** \return True on success. */
	virtual bool setFullscreen(bool fullscreen) = 0;

	//! Checks if the window could possibly be visible.
	virtual bool isWindowVisible() const = 0;

	//! Checks if the Irrlicht device supports touch events.
	virtual bool supportsTouchEvents() const = 0;

	//! Get the position of this window on screen
	virtual core::position2di getWindowPosition() = 0;

	//! Activate any joysticks, and generate events for them.
	virtual bool activateJoysticks(core::array<SJoystickInfo> &joystickInfo) = 0;

	//! Get the SDL version
	virtual std::string getVersionString() const = 0;

	//! Get the display density in dots per inch.
	virtual float getDisplayDensity() const = 0;

	//! Activate accelerometer.
	virtual bool activateAccelerometer(float updateInterval = 0.016666f) = 0;

	//! Deactivate accelerometer.
	virtual bool deactivateAccelerometer() = 0;

	//! Is accelerometer active.
	virtual bool isAccelerometerActive() = 0;

	//! Is accelerometer available.
	virtual bool isAccelerometerAvailable() = 0;

	//! Activate gyroscope.
	virtual bool activateGyroscope(float updateInterval = 0.016666f) = 0;

	//! Deactivate gyroscope.
	virtual bool deactivateGyroscope() = 0;

	//! Is gyroscope active.
	virtual bool isGyroscopeActive() = 0;

	//! Is gyroscope available.
	virtual bool isGyroscopeAvailable() = 0;

	//! Activate device motion.
	virtual bool activateDeviceMotion(float updateInterval = 0.016666f) = 0;

	//! Deactivate device motion.
	virtual bool deactivateDeviceMotion() = 0;

	//! Is device motion active.
	virtual bool isDeviceMotionActive() = 0;

	//! Is device motion available.
	virtual bool isDeviceMotionAvailable() = 0;

	//! Set the maximal elapsed time between 2 clicks to generate doubleclicks for the mouse. It also affects tripleclick behavior.
	virtual void setDoubleClickTime(u32 timeMs) = 0;

	//! Get the maximal elapsed time between 2 clicks to generate double- and tripleclicks for the mouse.
	virtual u32 getDoubleClickTime() const = 0;

	//! Remove all messages pending in the system message loop
	virtual void clearSystemMessages() = 0;

	//! Resize the render window.
	virtual void setWindowSize(const core::dimension2d<u32> &size) = 0;

	virtual bool swapBuffers() = 0;

	//! returns the video driver
	virtual video::VideoDriver *getVideoDriver() = 0;

	//! return file system
	virtual io::IFileSystem *getFileSystem() = 0;

	//! returns the gui environment
	virtual gui::IGUIEnvironment *getGUIEnvironment() = 0;

	//! returns the scene manager
	virtual scene::ISceneManager *getSceneManager() = 0;

	//! \return Returns a pointer to the mouse cursor control interface.
	virtual gui::ICursorControl *getCursorControl() = 0;

	//! send the event to the right receiver
	virtual bool postEventFromUser(const SEvent &event) = 0;

	//! Sets a new event receiver to receive events
	virtual void setEventReceiver(IEventReceiver *receiver) = 0;

	//! Returns pointer to the current event receiver. Returns 0 if there is none.
	virtual IEventReceiver *getEventReceiver() = 0;

	//! Sets the input receiving scene manager.
	/** If set to null, the main scene manager (returned by GetSceneManager()) will receive the input */
	virtual void setInputReceivingSceneManager(scene::ISceneManager *sceneManager) = 0;

	//! Returns a pointer to the logger.
	virtual os::Logger *getLogger() = 0;

	//! Returns the operation system opertator object.
	virtual os::Clipboard *getOSOperator() = 0;

	virtual std::variant<u32, EKEY_CODE> getScancodeFromKey(const Keycode &key) const = 0;
	virtual Keycode getKeyFromScancode(const u32 scancode) const = 0;
};

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
ISDLDevice *createDevice(
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
ISDLDevice *createDeviceEx(
	const SDLDeviceParameters &parameters);
