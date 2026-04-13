// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Utils/IReferenceCounted.h"
#include "Utils/irrString.h"

namespace os {

//! The OSOperator provides OS-specific methods and information.
class Clipboard : public virtual IReferenceCounted
{
public:
	Clipboard() = default;
	~Clipboard();

	Clipboard(const Clipboard &) = delete;
	Clipboard &operator=(const Clipboard &) = delete;

	//! Copies text to the clipboard
	//! \param text: text in utf-8
	void copyToClipboard(const c8 *text) const;

	//! Copies text to the primary selection
	//! This is a no-op on some platforms.
	//! \param text: text in utf-8
	void copyToPrimarySelection(const c8 *text) const;

	//! Get text from the clipboard
	//! \return Returns 0 if no string is in there, otherwise an utf-8 string.
	const c8 *getTextFromClipboard() const;

	//! Get text from the primary selection
	//! This is a no-op on some platforms.
	//! \return Returns 0 if no string is in there, otherwise an utf-8 string.
	const c8 *getTextFromPrimarySelection() const;

private:
	// These need to be freed with SDL_free
	mutable char *ClipboardSelectionText = nullptr;
	mutable char *PrimarySelectionText = nullptr;
};

}
