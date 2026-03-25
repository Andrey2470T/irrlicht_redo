// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "IReferenceCounted.h"
#include "position2d.h"
#include "rect.h"


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

//! structure used to set sprites as cursors.
struct SCursorSprite
{
	SCursorSprite() :
			SpriteBank(0), SpriteId(-1)
	{
	}

	SCursorSprite(gui::IGUISpriteBank *spriteBank, s32 spriteId, const core::position2d<s32> &hotspot = (core::position2d<s32>(0, 0))) :
			SpriteBank(spriteBank), SpriteId(spriteId), HotSpot(hotspot)
	{
	}

	IGUISpriteBank *SpriteBank;
	s32 SpriteId;
	core::position2d<s32> HotSpot;
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
		CursorControl(SDLDevice *dev) :
				Device(dev), IsVisible(true)
		{
			initCursors();
		}

		//! Changes the visible state of the mouse cursor.
		void setVisible(bool visible) override
		{
			IsVisible = visible;
#ifdef _IRR_USE_SDL3_
			if (visible)
				SDL_ShowCursor();
			else
				SDL_HideCursor();
#else
			SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
#endif
		}

		//! Returns if the cursor is currently visible.
		bool isVisible() const override
		{
			return IsVisible;
		}

		//! Sets the new position of the cursor.
		void setPosition(const core::position2d<f32> &pos) override
		{
			setPosition(pos.X, pos.Y);
		}

		//! Sets the new position of the cursor.
		void setPosition(f32 x, f32 y) override
		{
			setPosition((s32)(x * Device->Width), (s32)(y * Device->Height));
		}

		//! Sets the new position of the cursor.
		void setPosition(const core::position2d<s32> &pos) override
		{
			setPosition(pos.X, pos.Y);
		}

		//! Sets the new position of the cursor.
		void setPosition(s32 x, s32 y) override
		{
#ifndef __ANDROID__
			// On Android, this somehow results in a camera jump when enabling
			// relative mouse mode and it isn't supported anyway.
			SDL_WarpMouseInWindow(Device->Window,
					static_cast<int>(x / Device->ScaleX),
					static_cast<int>(y / Device->ScaleY));
#endif
#ifdef _IRR_USE_SDL3_
			if (SDL_GetWindowRelativeMouseMode(Device->Window)) {
#else
			if (SDL_GetRelativeMouseMode()) {
#endif
				// There won't be an event for this warp (details on libsdl-org/SDL/issues/6034)
				Device->MouseX = x;
				Device->MouseY = y;
			}
		}

		//! Returns the current position of the mouse cursor.
		const core::position2d<s32> &getPosition(bool updateCursor) override
		{
			if (updateCursor)
				updateCursorPos();
			return CursorPos;
		}

		//! Returns the current position of the mouse cursor.
		core::position2d<f32> getRelativePosition(bool updateCursor) override
		{
			if (updateCursor)
				updateCursorPos();
			return core::position2d<f32>(CursorPos.X / (f32)Device->Width,
					CursorPos.Y / (f32)Device->Height);
		}

		void setReferenceRect(core::rect<s32> *rect = 0) override
		{
		}

		virtual void setRelativeMode(bool relative) override
		{
#ifdef _IRR_USE_SDL3_
			if (relative != (bool)SDL_GetWindowRelativeMouseMode(Device->Window)) {
				SDL_SetWindowRelativeMouseMode(Device->Window, relative);
			}
#else
			// Only change it when necessary, as it flushes mouse motion when enabled
			if (relative != static_cast<bool>(SDL_GetRelativeMouseMode())) {
				SDL_SetRelativeMouseMode(relative ? SDL_TRUE : SDL_FALSE);
			}
#endif
		}

		void setActiveIcon(gui::ECURSOR_ICON iconId) override
		{
			ActiveIcon = iconId;
			if (iconId > Cursors.size() || !Cursors[iconId]) {
				iconId = gui::ECI_NORMAL;
				if (iconId > Cursors.size() || !Cursors[iconId])
					return;
			}
			SDL_SetCursor(Cursors[iconId].get());
		}

		gui::ECURSOR_ICON getActiveIcon() const override
		{
			return ActiveIcon;
		}

	private:
		void updateCursorPos()
		{
#ifdef _IRR_EMSCRIPTEN_PLATFORM_
			EmscriptenPointerlockChangeEvent pointerlockStatus; // let's hope that test is not expensive ...
			if (emscripten_get_pointerlock_status(&pointerlockStatus) == EMSCRIPTEN_RESULT_SUCCESS) {
				if (pointerlockStatus.isActive) {
					CursorPos.X += Device->MouseXRel;
					CursorPos.Y += Device->MouseYRel;
					Device->MouseXRel = 0;
					Device->MouseYRel = 0;
				} else {
					CursorPos.X = Device->MouseX;
					CursorPos.Y = Device->MouseY;
				}
			}
#else
			CursorPos.X = Device->MouseX;
			CursorPos.Y = Device->MouseY;

			if (CursorPos.X < 0)
				CursorPos.X = 0;
			if (CursorPos.X > (s32)Device->Width)
				CursorPos.X = Device->Width;
			if (CursorPos.Y < 0)
				CursorPos.Y = 0;
			if (CursorPos.Y > (s32)Device->Height)
				CursorPos.Y = Device->Height;
#endif
		}

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
