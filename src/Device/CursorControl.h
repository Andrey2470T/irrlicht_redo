// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "Device/ICursorControl.h"
#include "Utils/rect.h"
#include <vector>
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

//! Interface to manipulate the mouse cursor.
class CursorControl : public ICursorControl
{
public:
	CursorControl(SDLDevice *dev);

	//! Changes the visible state of the mouse cursor.
	void setVisible(bool visible) override;

	//! Returns if the cursor is currently visible.
	bool isVisible() const override
	{
		return IsVisible;
	}

	//! Sets the new position of the cursor.
	void setPosition(f32 x, f32 y) override;

	//! Sets the new position of the cursor.
	void setPosition(s32 x, s32 y) override;

	//! Returns the current position of the mouse cursor.
	const core::position2d<s32> &getPosition(bool updateCursor=true) override;

	//! Returns the current position of the mouse cursor.
	core::position2d<f32> getRelativePosition(bool updateCursor=true) override;

	void setRelativeMode(bool relative) override;

	void setActiveIcon(gui::ECURSOR_ICON iconId) override;

	gui::ECURSOR_ICON getActiveIcon() const override
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
