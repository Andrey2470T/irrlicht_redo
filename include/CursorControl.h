// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "IReferenceCounted.h"
#include "position2d.h"
#include "rect.h"
#include <memory>

#ifdef _IRR_USE_SDL3_
#define SDL_DISABLE_OLD_NAMES
#include <SDL3/SDL.h>
#include <SDL3/SDL_mouse.h>
#else
#include <SDL.h>

#define SDL_SYSTEM_CURSOR_DEFAULT SDL_SYSTEM_CURSOR_ARROW
#define SDL_SYSTEM_CURSOR_POINTER SDL_SYSTEM_CURSOR_HAND
#define SDL_SYSTEM_CURSOR_TEXT SDL_SYSTEM_CURSOR_IBEAM
#define SDL_SYSTEM_CURSOR_NOT_ALLOWED SDL_SYSTEM_CURSOR_NO
#define SDL_SYSTEM_CURSOR_MOVE SDL_SYSTEM_CURSOR_SIZEALL
#define SDL_SYSTEM_CURSOR_NESW_RESIZE SDL_SYSTEM_CURSOR_SIZENESW
#define SDL_SYSTEM_CURSOR_NWSE_RESIZE SDL_SYSTEM_CURSOR_SIZENWSE
#define SDL_SYSTEM_CURSOR_NS_RESIZE SDL_SYSTEM_CURSOR_SIZENS
#define SDL_SYSTEM_CURSOR_EW_RESIZE SDL_SYSTEM_CURSOR_SIZEWE
#endif

class SDLDevice;

namespace gui
{

class IGUISpriteBank;

//! Default icons for cursors
enum ECURSOR_ICON
{
	// Following cursors might be system specific, or might use an Irrlicht icon-set. No guarantees so far.
	ECI_NORMAL,   // arrow
	ECI_CROSS,    // Crosshair
	ECI_HAND,     // Hand
	ECI_HELP,     // Arrow and question mark
	ECI_IBEAM,    // typical text-selection cursor
	ECI_NO,       // should not click icon
	ECI_WAIT,     // hourglass
	ECI_SIZEALL,  // arrow in all directions
	ECI_SIZENESW, // resizes in direction north-east or south-west
	ECI_SIZENWSE, // resizes in direction north-west or south-east
	ECI_SIZENS,   // resizes in direction north or south
	ECI_SIZEWE,   // resizes in direction west or east
	ECI_UP,       // up-arrow

	// Implementer note: Should we add system specific cursors, which use guaranteed the system icons,
	// then I would recommend using a naming scheme like ECI_W32_CROSS, ECI_X11_CROSSHAIR and adding those
	// additionally.

	ECI_COUNT // maximal of defined cursors. Note that higher values can be created at runtime
};

//! Names for ECURSOR_ICON
const c8 *const GUICursorIconNames[ECI_COUNT + 1] = {
		"normal",
		"cross",
		"hand",
		"help",
		"ibeam",
		"no",
		"wait",
		"sizeall",
		"sizenesw",
		"sizenwse",
		"sizens",
		"sizewe",
		"sizeup",
		0,
};

//! platform specific behavior flags for the cursor
enum ECURSOR_PLATFORM_BEHAVIOR
{
	//! default - no platform specific behavior
	ECPB_NONE = 0,

	//! On X11 try caching cursor updates as XQueryPointer calls can be expensive.
	/** Update cursor positions only when the irrlicht timer has been updated or the timer is stopped.
		This means you usually get one cursor update per device->run() which will be fine in most cases.
		See this forum-thread for a more detailed explanation:
		http://irrlicht.sourceforge.net/forum/viewtopic.php?f=7&t=45525
	*/
	ECPB_X11_CACHE_UPDATES = 1
};

//! Interface to manipulate the mouse cursor.
class CursorControl : public virtual IReferenceCounted
{
public:
		CursorControl(SDLDevice *dev);

		//! Changes the visible state of the mouse cursor.
		void setVisible(bool visible);

		//! Returns if the cursor is currently visible.
		bool isVisible() const
		{
			return IsVisible;
		}

		//! Sets the new position of the cursor.
		void setPosition(const core::position2d<f32> &pos)
		{
			setPosition(pos.X, pos.Y);
		}

		//! Sets the new position of the cursor.
		void setPosition(f32 x, f32 y);

		//! Sets the new position of the cursor.
		void setPosition(const core::position2d<s32> &pos)
		{
			setPosition(pos.X, pos.Y);
		}

		//! Sets the new position of the cursor.
		void setPosition(s32 x, s32 y);

		//! Returns the current position of the mouse cursor.
		const core::position2d<s32> &getPosition(bool updateCursor=true);

		//! Returns the current position of the mouse cursor.
		core::position2d<f32> getRelativePosition(bool updateCursor=true);

		void setRelativeMode(bool relative);

		void setActiveIcon(gui::ECURSOR_ICON iconId);

		gui::ECURSOR_ICON getActiveIcon() const
		{
			return ActiveIcon;
		}

	private:
		void updateCursorPos();

		void initCursors();

		SDLDevice *Device;
		core::position2d<s32> CursorPos;
		bool IsVisible;

		struct CursorDeleter
		{
			void operator()(SDL_Cursor *ptr)
			{
#ifdef _IRR_USE_SDL3_
				if (ptr)
					SDL_DestroyCursor(ptr);
#else
				if (ptr)
					SDL_FreeCursor(ptr);
#endif
			}
		};
		std::vector<std::unique_ptr<SDL_Cursor, CursorDeleter>> Cursors;
		gui::ECURSOR_ICON ActiveIcon = gui::ECURSOR_ICON::ECI_NORMAL;
	};

} // end namespace gui
