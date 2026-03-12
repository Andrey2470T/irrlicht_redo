// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "Clipboard.h"

#include <SDL_clipboard.h>
#include <SDL_version.h>

namespace os {

Clipboard::~Clipboard()
{
	SDL_free(ClipboardSelectionText);
	SDL_free(PrimarySelectionText);
}

//! copies text to the clipboard
void Clipboard::copyToClipboard(const c8 *text) const
{
	if (strlen(text) == 0)
		return;

	SDL_SetClipboardText(text);
}

//! copies text to the primary selection
void Clipboard::copyToPrimarySelection(const c8 *text) const
{
	if (strlen(text) == 0)
		return;

#if SDL_VERSION_ATLEAST(2, 25, 0)
	SDL_SetPrimarySelectionText(text);
#endif
}

//! gets text from the clipboard
const c8 *Clipboard::getTextFromClipboard() const
{
	SDL_free(ClipboardSelectionText);
	ClipboardSelectionText = SDL_GetClipboardText();
	return ClipboardSelectionText;
}

//! gets text from the primary selection
const c8 *Clipboard::getTextFromPrimarySelection() const
{
#if SDL_VERSION_ATLEAST(2, 25, 0)
	SDL_free(PrimarySelectionText);
	PrimarySelectionText = SDL_GetPrimarySelectionText();
	return PrimarySelectionText;
#else
	return 0;
#endif
}

}
