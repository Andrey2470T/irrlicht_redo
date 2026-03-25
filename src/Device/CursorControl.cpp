#include "CursorControl.h"
#include "SDLDevice.h"

namespace gui
{

CursorControl::CursorControl(SDLDevice *dev) :
		Device(dev), IsVisible(true)
{
	initCursors();
}

//! Changes the visible state of the mouse cursor.
void CursorControl::setVisible(bool visible)
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

//! Sets the new position of the cursor.
void CursorControl::setPosition(f32 x, f32 y)
{
	setPosition((s32)(x * Device->Width), (s32)(y * Device->Height));
}

//! Sets the new position of the cursor.
void CursorControl::setPosition(s32 x, s32 y)
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
const core::position2d<s32> &CursorControl::getPosition(bool updateCursor)
{
	if (updateCursor)
		updateCursorPos();
	return CursorPos;
}

//! Returns the current position of the mouse cursor.
core::position2d<f32> CursorControl::getRelativePosition(bool updateCursor)
{
	if (updateCursor)
		updateCursorPos();
	return core::position2d<f32>(CursorPos.X / (f32)Device->Width,
			CursorPos.Y / (f32)Device->Height);
}

void CursorControl::setRelativeMode(bool relative)
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

void CursorControl::setActiveIcon(gui::ECURSOR_ICON iconId)
{
	ActiveIcon = iconId;
	if (iconId > Cursors.size() || !Cursors[iconId]) {
		iconId = gui::ECI_NORMAL;
		if (iconId > Cursors.size() || !Cursors[iconId])
			return;
	}
	SDL_SetCursor(Cursors[iconId].get());
}

void CursorControl::updateCursorPos()
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

void CursorControl::initCursors()
{
	Cursors.reserve(gui::ECI_COUNT);

	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));     // ECI_NORMAL
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR));   // ECI_CROSS
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER));     // ECI_HAND
	Cursors.emplace_back(nullptr);                                               // ECI_HELP
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT));        // ECI_IBEAM
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED)); // ECI_NO
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT));        // ECI_WAIT
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE));        // ECI_SIZEALL
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE)); // ECI_SIZENESW
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE)); // ECI_SIZENWSE
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE));   // ECI_SIZENS
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE));   // ECI_SIZEWE
	Cursors.emplace_back(nullptr);                                               // ECI_UP
}

}
