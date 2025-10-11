#pragma once

#include "BasicIncludes.h"
#include "Keycodes.h"
#include <string.h>
#include "LogStream.h"
#include <optional>

namespace core
{

//! Enumeration for all event types there are.
enum EventType
{
	ET_GUI_EVENT = 1,
	ET_MOUSE_INPUT_EVENT,
	ET_KEY_INPUT_EVENT,
	ET_STRING_INPUT_EVENT,
	ET_TOUCH_INPUT_EVENT,
	ET_ACCELEROMETER_EVENT,
	ET_GYROSCOPE_EVENT,
	ET_JOYSTICK_INPUT_EVENT,
    ET_LOG_TEXT_EVENT,
	ET_USER_EVENT,
    ET_APPLICATION_EVENT
};

#define EET_GUI_EVENT ET_GUI_EVENT
#define EET_MOUSE_INPUT_EVENT ET_MOUSE_INPUT_EVENT
#define EET_KEY_INPUT_EVENT ET_KEY_INPUT_EVENT
#define EET_STRING_INPUT_EVENT ET_STRING_INPUT_EVENT
#define EET_TOUCH_INPUT_EVENT ET_TOUCH_INPUT_EVENT
#define EET_ACCELEROMETER_EVENT ET_ACCELEROMETER_EVENT
#define EET_GYROSCOPE_EVENT ET_GYROSCOPE_EVENT
#define EET_JOYSTICK_INPUT_EVENT ET_JOYSTICK_INPUT_EVENT
#define EET_LOG_TEXT_EVENT ET_LOG_TEXT_EVENT
#define EET_USER_EVENT ET_USER_EVENT
#define EET_APPLICATION_EVENT ET_APPLICATION_EVENT

//! Enumeration for all mouse input events
enum MouseInputEventType
{
	MIE_LMOUSE_PRESSED_DOWN = 0,
	MIE_RMOUSE_PRESSED_DOWN,
	MIE_MMOUSE_PRESSED_DOWN,
	MIE_LMOUSE_LEFT_UP,
	MIE_RMOUSE_LEFT_UP,
	MIE_MMOUSE_LEFT_UP,
	MIE_MOUSE_MOVED,
	MIE_MOUSE_WHEEL,
	MIE_LMOUSE_DOUBLE_CLICK,
	MIE_RMOUSE_DOUBLE_CLICK,
	MIE_MMOUSE_DOUBLE_CLICK,
	MIE_LMOUSE_TRIPLE_CLICK,
	MIE_RMOUSE_TRIPLE_CLICK,
	MIE_MMOUSE_TRIPLE_CLICK,
	//! Mouse enters canvas used for rendering.
	//! Only generated on emscripten
	MIE_MOUSE_ENTER_CANVAS,
	//! Mouse leaves canvas used for rendering.
	//! Only generated on emscripten
	MIE_MOUSE_LEAVE_CANVAS,
	//! No real event. Just for convenience to get number of events
	MIE_COUNT
};

#define EMIE_LMOUSE_PRESSED_DOWN MIE_LMOUSE_PRESSED_DOWN
#define EMIE_RMOUSE_PRESSED_DOWN MIE_RMOUSE_PRESSED_DOWN
#define EMIE_MMOUSE_PRESSED_DOWN MIE_MMOUSE_PRESSED_DOWN
#define EMIE_LMOUSE_LEFT_UP MIE_LMOUSE_LEFT_UP
#define EMIE_RMOUSE_LEFT_UP MIE_RMOUSE_LEFT_UP
#define WMIE_MMOUSE_LEFT_UP MIE_MMOUSE_LEFT_UP
#define EMIE_MOUSE_MOVED MIE_MOUSE_MOVED
#define EMIE_MOUSE_WHEEL MIE_MOUSE_WHEEL
#define EMIE_LMOUSE_DOUBLE_CLICK MIE_LMOUSE_DOUBLE_CLICK
#define EMIE_RMOUSE_DOUBLE_CLICK MIE_RMOUSE_DOUBLE_CLICK
#define EMIE_MMOUSE_DOUBLE_CLICK MIE_MMOUSE_DOUBLE_CLICK
#define EMIE_LMOUSE_TRIPLE_CLICK MIE_LMOUSE_TRIPLE_CLICK
#define EMIE_RMOUSE_TRIPLE_CLICK MIE_RMOUSE_TRIPLE_CLICK
#define EMIE_MMOUSE_TRIPLE_CLICK MIE_MMOUSE_TRIPLE_CLICK
#define EMIE_MOUSE_ENTER_CANVAS MIE_MOUSE_ENTER_CANVAS
#define EMIE_MOUSE_LEAVE_CANVAS MIE_MOUSE_LEAVE_CANVAS
#define EMIE_COUNT MIE_COUNT

//! Masks for mouse button states
enum MouseButtonStateMask
{
	MBSM_LEFT = 0x01,
	MBSM_RIGHT = 0x02,
	MBSM_MIDDLE = 0x04,
	//! currently only on windows
	MBSM_EXTRA1 = 0x08,
	//! currently only on windows
	MBSM_EXTRA2 = 0x10,

	MBSM_FORCE_32_BIT = 0x7fffffff
};

//! Enumeration for all touch input events
enum TouchInputEventType
{
	TIE_PRESSED_DOWN = 0,
	TIE_LEFT_UP,
	TIE_MOVED,
	//! No real event. Just for convenience to get number of events
	TIE_COUNT
};

//! Enumeration for a commonly used application state events (it's useful mainly for mobile devices)
enum ApplicationEventType
{
	AET_WILL_RESUME = 0,
	AET_DID_RESUME,
	AET_WILL_PAUSE,
	AET_DID_PAUSE,
	AET_WILL_TERMINATE,
	AET_MEMORY_WARNING,
	AET_DPI_CHANGED,
	//! No real event, but to get number of event types.
	AET_COUNT
};

//! Enumeration for all events which are sendable by the gui system
enum GUIEventType
{
	//! A gui element has lost its focus.
	/** GUIEvent.Caller is losing the focus to GUIEvent.Element.
	If the event is absorbed then the focus will not be changed. */
	GET_ELEMENT_FOCUS_LOST = 0,

	//! A gui element has got the focus.
	/** If the event is absorbed then the focus will not be changed. */
	GET_ELEMENT_FOCUSED,

	//! The mouse cursor hovered over a gui element.
	/** If an element has sub-elements you also get this message for the subelements */
	GET_ELEMENT_HOVERED,

	//! The mouse cursor left the hovered element.
	/** If an element has sub-elements you also get this message for the subelements */
	GET_ELEMENT_LEFT,

	//! An element would like to close.
	/** Windows and context menus use this event when they would like to close,
	this can be canceled by absorbing the event. */
	GET_ELEMENT_CLOSED,

	//! A button was clicked.
	GET_BUTTON_CLICKED,

	//! A scrollbar has changed its position.
	GET_SCROLL_BAR_CHANGED,

	//! A checkbox has changed its check state.
	GET_CHECKBOX_CHANGED,

	//! A listbox would like to open.
	/** You can prevent the listbox from opening by absorbing the event. */
	GET_LISTBOX_OPENED,

	//! A new item in a listbox was selected.
	/** NOTE: You also get this event currently when the same item was clicked again after more than 500 ms. */
	GET_LISTBOX_CHANGED,

	//! An item in the listbox was selected, which was already selected.
	/** NOTE: You get the event currently only if the item was clicked again within 500 ms or selected by "enter" or "space". */
	GET_LISTBOX_SELECTED_AGAIN,

	//! A file has been selected in the file dialog
	GET_FILE_SELECTED,

	//! A directory has been selected in the file dialog
	GET_DIRECTORY_SELECTED,

	//! A file open dialog has been closed without choosing a file
	GET_FILE_CHOOSE_DIALOG_CANCELLED,

	//! In an editbox 'ENTER' was pressed
	GET_EDITBOX_ENTER,

	//! The text in an editbox was changed. This does not include automatic changes in text-breaking.
	GET_EDITBOX_CHANGED,

	//! The marked area in an editbox was changed.
	GET_EDITBOX_MARKING_CHANGED,

	//! The tab was changed in an tab control
	GET_TAB_CHANGED,

	//! The selection in a combo box has been changed
	GET_COMBO_BOX_CHANGED,

	//! A table has changed
	GET_TABLE_CHANGED,
	GET_TABLE_HEADER_CHANGED,
	GET_TABLE_SELECTED_AGAIN,

	//! No real event. Just for convenience to get number of events
	GET_COUNT
};

#define EGUI_EVENT_TYPE GUIEventType

#define EGET_ELEMENT_FOCUS_LOST GET_ELEMENT_FOCUS_LOST
#define EGET_ELEMENT_FOCUSED GET_ELEMENT_FOCUSED
#define EGET_ELEMENT_HOVERED GET_ELEMENT_HOVERED
#define EGET_ELEMENT_LEFT GET_ELEMENT_LEFT
#define EGET_ELEMENT_CLOSED GET_ELEMENT_CLOSED
#define EGET_BUTTON_CLICKED GET_BUTTON_CLICKED
#define EGET_SCROLL_BAR_CHANGED GET_SCROLL_BAR_CHANGED
#define EGET_CHECKBOX_CHANGED GET_CHECKBOX_CHANGED
#define EGET_LISTBOX_OPENED GET_LISTBOX_OPENED
#define EGET_LISTBOX_CHANGED GET_LISTBOX_CHANGED
#define EGET_LISTBOX_SELECTED_AGAIN GET_LISTBOX_SELECTED_AGAIN
#define EGET_FILE_SELECTED GET_FILE_SELECTED
#define EGET_DIRECTORY_SELECTED GET_DIRECTORY_SELECTED
#define EGET_FILE_CHOOSE_DIALOG_CANCELLED GET_FILE_CHOOSE_DIALOG_CANCELLED
#define EGET_EDITBOX_ENTER GET_EDITBOX_ENTER
#define EGET_EDITBOX_CHANGED GET_EDITBOX_CHANGED
#define EGET_EDITBOX_MARKING_CHANGED GET_EDITBOX_MARKING_CHANGED
#define EGET_TAB_CHANGED GET_TAB_CHANGED
#define EGET_COMBO_BOX_CHANGED GET_COMBO_BOX_CHANGED
#define EGET_TABLE_CHANGED GET_TABLE_CHANGED
#define EGET_TABLE_HEADER_CHANGED GET_TABLE_HEADER_CHANGED
#define EGET_TABLE_SELECTED_AGAIN GET_TABLE_SELECTED_AGAIN

//! Holds information about an event
struct Event
{
	//! Any kind of GUI event.
	struct GUIEvent
	{
		//! IGUIElement who called the event
		std::optional<u32> Caller;

		//! If the event has something to do with another element, it will be held here.
		std::optional<u32> Element;

		//! Type of GUI Event
		GUIEventType Type;
	};

	//! Any kind of mouse event.
	struct MouseInputEvent
	{
		//! X position of mouse cursor
		s32 X;
		//! Y position of mouse cursor
		s32 Y;

		//! mouse wheel delta, often 1.0 or -1.0, but can have other values < 0.f or > 0.f;
		/** Only valid if event was EMIE_MOUSE_WHEEL */
		f32 WheelDelta;

		//! True if shift was also pressed
		bool Shift : 1;

		//! True if ctrl was also pressed
		bool Control : 1;

		//! Is this a simulated mouse event generated by the engine itself?
		bool Simulated : 1;

		//! A bitmap of button states. You can use isButtonPressed() to determine
		//! if a button is pressed or not.
		u32 ButtonStates;

		//! Is the left button pressed down?
		bool isLeftPressed() const { return 0 != (ButtonStates & MBSM_LEFT); }

		//! Is the right button pressed down?
		bool isRightPressed() const { return 0 != (ButtonStates & MBSM_RIGHT); }

		//! Is the middle button pressed down?
		bool isMiddlePressed() const { return 0 != (ButtonStates & MBSM_MIDDLE); }

		//! Type of mouse event
		MouseInputEventType Type;
	};

	//! Any kind of keyboard event.
	struct KeyInputEvent
	{
		//! Character corresponding to the key (0, if not a character, value undefined in key releases)
		wchar_t Char;

		//! Key which has been pressed or released
		KEY_CODE Key;

		//! System dependent code. Only set for systems which are described below, otherwise undefined.
		//! Android: int32_t with physical key as returned by AKeyEvent_getKeyCode
		u32 SystemKeyCode;

		//! If not true, then the key was left up
		bool PressedDown : 1;

		//! True if shift was also pressed
		bool Shift : 1;

		//! True if ctrl was also pressed
		bool Control : 1;
	};

	//! String input event.
    struct StringInputEvent
	{
		//! The string that is entered
        std::wstring *Str;
    };

	//! Any kind of touch event.
	struct TouchInputEvent
	{
		// Touch ID.
		size_t ID;

		// X position of simple touch.
		s32 X;
		// Y position of simple touch.
		s32 Y;

		// number of current touches
		s32 touchedCount;

		//! Type of touch event.
		TouchInputEventType Type;
	};

	//! Any kind of accelerometer event.
	struct AccelerometerEvent
	{
		// X acceleration.
		f64 X;
		// Y acceleration.
		f64 Y;
		// Z acceleration.
		f64 Z;
	};

	//! Any kind of gyroscope event.
	struct GyroscopeEvent
	{
		// X rotation.
		f64 X;
		// Y rotation.
		f64 Y;
		// Z rotation.
		f64 Z;
	};

	//! Any kind of device motion event.
	struct DeviceMotionEvent
	{
		// X angle - roll.
		f64 X;
		// Y angle - pitch.
		f64 Y;
		// Z angle - yaw.
		f64 Z;
	};

	//! A joystick event.
	/** Unlike other events, joystick events represent the result of polling
	 * each connected joystick once per run() of the device. Joystick events will
	 * not be generated by default.  If joystick support is available for the
	 * active device, and @ref irr::IrrlichtDevice::activateJoysticks() has been
	 * called, an event of this type will be generated once per joystick per
	 * @ref IrrlichtDevice::run() regardless of whether the state of the joystick
	 * has actually changed. */
	struct JoystickEvent
	{
		enum
		{
			NUMBER_OF_BUTTONS = 32,

			AXIS_X = 0, // e.g. analog stick 1 left to right
			AXIS_Y,     // e.g. analog stick 1 top to bottom
			AXIS_Z,     // e.g. throttle, or analog 2 stick 2 left to right
			AXIS_R,     // e.g. rudder, or analog 2 stick 2 top to bottom
			AXIS_U,
			AXIS_V,
			NUMBER_OF_AXES = 18 // (please tell Irrlicht maintainers if you absolutely need more axes)
		};

		/** A bitmap of button states.  You can use IsButtonPressed() to
		 ( check the state of each button from 0 to (NUMBER_OF_BUTTONS - 1) */
		u32 ButtonStates;

		/** For AXIS_X, AXIS_Y, AXIS_Z, AXIS_R, AXIS_U and AXIS_V
		 * Values are in the range -32768 to 32767, with 0 representing
		 * the center position.  You will receive the raw value from the
		 * joystick, and so will usually want to implement a dead zone around
		 * the center of the range. Axes not supported by this joystick will
		 * always have a value of 0. On Linux, POV hats are represented as axes,
		 * usually the last two active axis.
		 */
		s16 Axis[NUMBER_OF_AXES];

		/** The POV represents the angle of the POV hat in degrees * 100,
		 * from 0 to 35,900.  A value of 65535 indicates that the POV hat
		 * is centered (or not present).
		 * This value is only supported on Windows.  On Linux, the POV hat
		 * will be sent as 2 axes instead. */
		u16 POV;

		//! The ID of the joystick which generated this event.
		/** This is an internal Irrlicht index; it does not map directly
		 * to any particular hardware joystick. */
		u8 Joystick;

		//! A helper function to check if a button is pressed.
		bool IsButtonPressed(u32 button) const
		{
			if (button >= (u32)NUMBER_OF_BUTTONS)
				return false;

			return (ButtonStates & (1 << button)) ? true : false;
		}
	};

	//! Any kind of user event.
	struct UserEvent
	{
		//! Some user specified data as int
		size_t UserData1;
		//! Another user specified data as int
		size_t UserData2;
	};

    struct LogEvent
    {
        //! Pointer to text which has been logged
        const c8 *Text;

        //! Log level in which the text has been logged
        LogLevel Level;
    };

	// Application state event
	struct ApplicationEvent
	{
		ApplicationEventType Type;
	};

	EventType Type;
    union
	{
		struct GUIEvent GUI;
		struct MouseInputEvent MouseInput;
		struct KeyInputEvent KeyInput;
        struct StringInputEvent StringInput;
		struct TouchInputEvent TouchInput;
		struct AccelerometerEvent Accelerometer;
		struct GyroscopeEvent Gyroscope;
		struct DeviceMotionEvent DeviceMotion;
		struct JoystickEvent Joystick;
        struct LogEvent Log;
		struct UserEvent User;
		struct ApplicationEvent Application;
    };

    Event() {
        Type = static_cast<EventType>(0);
		// zero the biggest union member we have, which clears all others too
        memset(&Joystick, 0, sizeof(Joystick));
	}
};

//! Information on a joystick, returned from @ref irr::IrrlichtDevice::activateJoysticks()
struct JoystickInfo
{
    //! The ID of the joystick
    /** This is an internal Irrlicht index; it does not map directly
     * to any particular hardware joystick. It corresponds to the
     * irr::SJoystickEvent Joystick ID. */
    u8 Joystick;

    //! The name that the joystick uses to identify itself.
    std::string Name;

    //! The number of buttons that the joystick has.
    u32 Buttons;

    //! The number of axes that the joystick has, i.e. X, Y, Z, R, U, V.
    /** Note: with a Linux device, the POV hat (if any) will use two axes. These
     *  will be included in this count. */
    u32 Axes;

    //! An indication of whether the joystick has a POV hat.
    /** A Windows device will identify the presence or absence of the POV hat.
     *  A Linux device cannot, and will always return POV_HAT_UNKNOWN. */
    enum
    {
        //! A hat is definitely present.
        POV_HAT_PRESENT,

        //! A hat is definitely not present.
        POV_HAT_ABSENT,

        //! The presence or absence of a hat cannot be determined.
        POV_HAT_UNKNOWN
    } PovHat;
};

}
