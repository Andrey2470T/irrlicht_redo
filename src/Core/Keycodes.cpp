#include "Keycodes.h"

namespace core
{

bool keyIsKnownSpecial(KEY_CODE irrlichtKey)
{
	switch (irrlichtKey) {
	// keys which are known to have safe special character interpretation
	// could need changes over time (removals and additions!)
	case KEY_RETURN:
	case KEY_PAUSE:
	case KEY_ESCAPE:
	case KEY_PRIOR:
	case KEY_NEXT:
	case KEY_HOME:
	case KEY_END:
	case KEY_LEFT:
	case KEY_UP:
	case KEY_RIGHT:
	case KEY_DOWN:
	case KEY_TAB:
	case KEY_PRINT:
	case KEY_SNAPSHOT:
	case KEY_INSERT:
	case KEY_BACK:
	case KEY_DELETE:
	case KEY_HELP:
	case KEY_APPS:
	case KEY_SLEEP:
	case KEY_F1:
	case KEY_F2:
	case KEY_F3:
	case KEY_F4:
	case KEY_F5:
	case KEY_F6:
	case KEY_F7:
	case KEY_F8:
	case KEY_F9:
	case KEY_F10:
	case KEY_F11:
	case KEY_F12:
	case KEY_F13:
	case KEY_F14:
	case KEY_F15:
	case KEY_F16:
	case KEY_F17:
	case KEY_F18:
	case KEY_F19:
	case KEY_F20:
	case KEY_F21:
	case KEY_F22:
	case KEY_F23:
	case KEY_F24:
	case KEY_NUMLOCK:
	case KEY_SCROLL:
	case KEY_LCONTROL:
	case KEY_RCONTROL:
		return true;

	default:
		return false;
	}
}

int findCharToPassToIrrlicht(uint32_t sdlKey, KEY_CODE irrlichtKey, bool numlock)
{
	switch (irrlichtKey) {
	// special cases that always return a char regardless of how the SDL keycode
	// looks
	case KEY_RETURN:
	case KEY_ESCAPE:
		return (int)irrlichtKey;

	// This is necessary for keys on the numpad because they don't use the same
	// keycodes as their non-numpad versions (whose keycodes correspond to chars),
	// but have their own SDL keycodes and their own Irrlicht keycodes (which
	// don't correspond to chars).
	case KEY_MULTIPLY:
		return '*';
	case KEY_ADD:
		return '+';
	case KEY_SUBTRACT:
		return '-';
	case KEY_DIVIDE:
		return '/';

	default:
		break;
	}

	if (numlock) {
		// Number keys on the numpad are also affected, but we only want them
		// to produce number chars when numlock is enabled.
		switch (irrlichtKey) {
		case KEY_NUMPAD0:
			return '0';
		case KEY_NUMPAD1:
			return '1';
		case KEY_NUMPAD2:
			return '2';
		case KEY_NUMPAD3:
			return '3';
		case KEY_NUMPAD4:
			return '4';
		case KEY_NUMPAD5:
			return '5';
		case KEY_NUMPAD6:
			return '6';
		case KEY_NUMPAD7:
			return '7';
		case KEY_NUMPAD8:
			return '8';
		case KEY_NUMPAD9:
			return '9';
		default:
			break;
		}
	}

	// SDL in-place ORs values with no character representation with 1<<30
	// https://wiki.libsdl.org/SDL2/SDLKeycodeLookup
	// This also affects the numpad keys btw.
	if (sdlKey & (1 << 30))
		return 0;

	switch (irrlichtKey) {
	case KEY_PRIOR:
	case KEY_NEXT:
	case KEY_HOME:
	case KEY_END:
	case KEY_LEFT:
	case KEY_UP:
	case KEY_RIGHT:
	case KEY_DOWN:
	case KEY_NUMLOCK:
		return 0;
	default:
		return sdlKey;
	}
}
}
