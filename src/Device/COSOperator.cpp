// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "COSOperator.h"

// "SDL_version.h" for SDL_VERSION_ATLEAST
#ifdef _IRR_USE_SDL3_
	#include <SDL3/SDL_clipboard.h>
	#include <SDL3/SDL_version.h>
#else
	#include <SDL_clipboard.h>
	#include <SDL_version.h>
#endif

// constructor
COSOperator::COSOperator()
{}

COSOperator::~COSOperator()
{
	SDL_free(ClipboardSelectionText);
	SDL_free(PrimarySelectionText);
}

//! copies text to the clipboard
void COSOperator::copyToClipboard(const c8 *text) const
{
	if (strlen(text) == 0)
		return;

	SDL_SetClipboardText(text);
}

//! copies text to the primary selection
void COSOperator::copyToPrimarySelection(const c8 *text) const
{
	if (strlen(text) == 0)
		return;

#if SDL_VERSION_ATLEAST(2, 25, 0)
	SDL_SetPrimarySelectionText(text);
#endif
}

//! gets text from the clipboard
const c8 *COSOperator::getTextFromClipboard() const
{
	SDL_free(ClipboardSelectionText);
	ClipboardSelectionText = SDL_GetClipboardText();
	return ClipboardSelectionText;
}

//! gets text from the primary selection
const c8 *COSOperator::getTextFromPrimarySelection() const
{
#if SDL_VERSION_ATLEAST(2, 25, 0)
	SDL_free(PrimarySelectionText);
	PrimarySelectionText = SDL_GetPrimarySelectionText();
	return PrimarySelectionText;
#else
	return 0;
#endif
}
