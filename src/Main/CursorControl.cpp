#include "CursorControl.h"
#include "MainWindow.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

namespace main
{

CursorControl::CursorControl(MainWindow *window)
	: Pos(0, 0), IsVisible(true), Icon(CI_NORMAL), Wnd(window)
{
	initCursors();
}

CursorControl::~CursorControl()
{
	for (auto &cursor : Cursors)
		SDL_FreeCursor(cursor);
}

//! Sets the new position of the cursor.
void CursorControl::setPosition(utils::v2i newPos)
{
#ifndef __ANDROID__
	// On Android, this somehow results in a camera jump when enabling
	// relative mouse mode and it isn't supported anyway.
	SDL_WarpMouseInWindow(Wnd->Window,
			static_cast<int>(newPos.X / Wnd->Scale.X),
			static_cast<int>(newPos.Y / Wnd->Scale.Y));
#endif

	if (SDL_GetRelativeMouseMode()) {
		// There won't be an event for this warp (details on libsdl-org/SDL/issues/6034)
		Wnd->MousePos = newPos;
	}
}

//! Returns the current position of the mouse cursor.
const utils::v2i &CursorControl::getPosition(bool updateCursor)
{
    if (updateCursor)
		updateCursorPos();
    return Pos;
}

//! Returns the current position of the mouse cursor.
utils::v2f CursorControl::getRelativePosition(bool updateCursor)
{
	if (updateCursor)
		updateCursorPos();
    return utils::v2f(Pos.X / (f32)Wnd->Params.Width,
			Pos.Y / (f32)Wnd->Params.Height);
}

void CursorControl::setRelativeMode(bool relative)
{
	// Only change it when necessary, as it flushes mouse motion when enabled
	if (relative != static_cast<bool>(SDL_GetRelativeMouseMode())) {
		if (relative)
			SDL_SetRelativeMouseMode(SDL_TRUE);
		else
			SDL_SetRelativeMouseMode(SDL_FALSE);
	}
}

void CursorControl::setActiveIcon(CURSOR_ICON icon)
{
	Icon = icon;
	if (icon > Cursors.size() || !Cursors[icon])
		Icon = CI_NORMAL;

	SDL_SetCursor(Cursors[Icon]);
}

void CursorControl::updateCursorPos()
{
#ifdef EMSCRIPTEN
	EmscriptenPointerlockChangeEvent pointerlockStatus; // let's hope that test is not expensive ...
	if (emscripten_get_pointerlock_status(&pointerlockStatus) == EMSCRIPTEN_RESULT_SUCCESS) {
		if (pointerlockStatus.isActive) {
			Pos += Wnd->MouseRelPos;
			Wnd->MouseRelPos = utils::v2i();
		}
		else
			Pos = Wnd->MousePos;
	}
#else
    Pos.X = std::clamp(Wnd->MousePos.X, 0, (s32)Wnd->Params.Width);
    Pos.Y = std::clamp(Wnd->MousePos.Y, 0, (s32)Wnd->Params.Height);
#endif
}

void CursorControl::initCursors()
{
	Cursors.reserve(CI_COUNT);

	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));     // ECI_NORMAL
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR)); // ECI_CROSS
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));      // ECI_HAND
	Cursors.emplace_back(nullptr);                                             // ECI_HELP
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM));     // ECI_IBEAM
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO));        // ECI_NO
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT));      // ECI_WAIT
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL));   // ECI_SIZEALL
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW));  // ECI_SIZENESW
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE));  // ECI_SIZENWSE
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS));    // ECI_SIZENS
	Cursors.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE));    // ECI_SIZEWE
	Cursors.emplace_back(nullptr);                                             // ECI_UP
}

}
