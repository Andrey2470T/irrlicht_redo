// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Utils/IReferenceCounted.h"
#include "Utils/position2d.h"

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

//! Interface to manipulate the mouse cursor.
class ICursorControl : public virtual IReferenceCounted
{
public:
	ICursorControl() = default;
	virtual ~ICursorControl() = default;

	//! Changes the visible state of the mouse cursor.
	virtual void setVisible(bool visible) = 0;

	//! Returns if the cursor is currently visible.
	virtual bool isVisible() const = 0;

	//! Sets the new position of the cursor.
	void setPosition(const core::position2d<f32> &pos)
	{
		setPosition(pos.X, pos.Y);
	}

	//! Sets the new position of the cursor.
	virtual void setPosition(f32 x, f32 y) = 0;

	//! Sets the new position of the cursor.
	void setPosition(const core::position2d<s32> &pos)
	{
		setPosition(pos.X, pos.Y);
	}

	//! Sets the new position of the cursor.
	virtual void setPosition(s32 x, s32 y) = 0;

	//! Returns the current position of the mouse cursor.
	virtual const core::position2d<s32> &getPosition(bool updateCursor=true) = 0;

	//! Returns the current position of the mouse cursor.
	virtual core::position2d<f32> getRelativePosition(bool updateCursor=true) = 0;

	virtual void setRelativeMode(bool relative) = 0;

	virtual void setActiveIcon(gui::ECURSOR_ICON iconId) = 0;

	virtual gui::ECURSOR_ICON getActiveIcon() const = 0;
};

} // end namespace gui
