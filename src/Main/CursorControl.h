#pragma once

#include "utils/Vector2D.h"
#include "utils/Rect.h"

namespace main
{

//! Default icons for cursors
enum CURSOR_ICON
{
	// Following cursors might be system specific, or might use an Irrlicht icon-set. No guarantees so far.
	CI_NORMAL,   // arrow
	CI_CROSS,    // Crosshair
	CI_HAND,     // Hand
	CI_HELP,     // Arrow and question mark
	CI_IBEAM,    // typical text-selection cursor
	CI_NO,       // should not click icon
	CI_WAIT,     // hourglass
	CI_SIZEALL,  // arrow in all directions
	CI_SIZENESW, // resizes in direction north-east or south-west
	CI_SIZENWSE, // resizes in direction north-west or south-east
	CI_SIZENS,   // resizes in direction north or south
	CI_SIZEWE,   // resizes in direction west or east
	CI_UP,       // up-arrow

	// Implementer note: Should we add system specific cursors, which use guaranteed the system icons,
	// then I would recommend using a naming scheme like ECI_W32_CROSS, ECI_X11_CROSSHAIR and adding those
	// additionally.

	ECI_COUNT // maximal of defined cursors. Note that higher values can be created at runtime
};

class CursorControl
{
	utils::v2i Pos;
	bool IsVisible;
	CURSOR_ICON Icon;
	
	struct CursorDeleter
	{
		void operator()(SDL_Cursor *ptr)
		{
			if (ptr)
				SDL_FreeCursor(ptr);
		}
	};
		
	std::vector<std::unique_ptr<SDL_Cursor, CursorDeleter>> Cursors;
	
	SDLApp *App;
public:
	CursorControl(SDLApp *app);
	
	//! Changes the visible state of the mouse cursor.
	void setVisible(bool visible);

	//! Returns if the cursor is currently visible.
	bool isVisible() const
	{
		return IsVisible;
	}
	
	//! Sets the new position of the cursor.
	void setPosition(utils::v2i newPos);

	//! Returns the current position of the mouse cursor.
	const utils::v2i &getPosition(bool updateCursor);

	//! Returns the current position of the mouse cursor.
	utils::v2f getRelativePosition(bool updateCursor);

	void setRelativeMode(bool relative);

	void setActiveIcon(CURSOR_ICON icon);

	CURSOR_ICON getCurrentIcon() const
	{
		return Icon;
	}

private:
	void updateCursorPos();

	void initCursors();
};

}
