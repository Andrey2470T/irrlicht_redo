// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CNSOGLManager.h"

#ifdef _IRR_COMPILE_WITH_NSOGL_MANAGER_

#include <mach-o/dyld.h>
#include "os.h"

namespace irr
{
namespace video
{

CNSOGLManager::CNSOGLManager() :
		PrimaryContext(SExposedVideoData(0)), PixelFormat(nil)
{
#ifdef _DEBUG
	setDebugName("CNSOGLManager");
#endif
}

CNSOGLManager::~CNSOGLManager()
{
}

bool CNSOGLManager::initialize(const SIrrlichtCreationParameters &params, const SExposedVideoData &videodata)
{
	Params = params;

	return true;
}

void CNSOGLManager::terminate()
{
}

bool CNSOGLManager::generateSurface()
{
	return true;
}

void CNSOGLManager::destroySurface()
{
	[PixelFormat release];
	PixelFormat = nil;
}

bool CNSOGLManager::generateContext()
{
	NSOpenGLContext *Context = [[NSOpenGLContext alloc] initWithFormat:PixelFormat shareContext:nil];

	GLint Vsync = Params.Vsync ? 1 : 0;
	[Context setValues:&Vsync forParameter:NSOpenGLCPSwapInterval];

	if (Context == nil) {
		os::Printer::log("Could not create OpenGL context.", ELL_ERROR);
		return false;
	}

	// set exposed data
	CurrentContext.OpenGLOSX.Context = Context;

	if (!PrimaryContext.OpenGLOSX.Context)
		PrimaryContext.OpenGLOSX.Context = CurrentContext.OpenGLOSX.Context;

	return true;
}

const SExposedVideoData &CNSOGLManager::getContext() const
{
	return CurrentContext;
}

bool CNSOGLManager::activateContext(const SExposedVideoData &videoData, bool restorePrimaryOnZero)
{
	// TODO: handle restorePrimaryOnZero
	if (videoData.OpenGLOSX.Context) {
		if ((NSOpenGLContext *)videoData.OpenGLOSX.Context != [NSOpenGLContext currentContext]) {
			[(NSOpenGLContext *)videoData.OpenGLOSX.Context makeCurrentContext];

			CurrentContext = videoData;
		}
	}
	// set back to main context
	else {
		if ((NSOpenGLContext *)PrimaryContext.OpenGLOSX.Context != [NSOpenGLContext currentContext]) {
			[(NSOpenGLContext *)PrimaryContext.OpenGLOSX.Context makeCurrentContext];

			CurrentContext = PrimaryContext;
		}
	}

	return true;
}

void CNSOGLManager::destroyContext()
{
	if (CurrentContext.OpenGLOSX.Context) {
		if (PrimaryContext.OpenGLOSX.Context == CurrentContext.OpenGLOSX.Context)
			PrimaryContext.OpenGLOSX.Context = nil;

		[(NSOpenGLContext *)CurrentContext.OpenGLOSX.Context makeCurrentContext];
		[(NSOpenGLContext *)CurrentContext.OpenGLOSX.Context clearDrawable];
		[(NSOpenGLContext *)CurrentContext.OpenGLOSX.Context release];
		[NSOpenGLContext clearCurrentContext];

		CurrentContext.OpenGLOSX.Context = nil;
	}
}

// It appears that there is no separate GL proc address getter on OSX.
// https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/opengl_entrypts/opengl_entrypts.html
void *CNSOGLManager::getProcAddress(const std::string &procName)
{
	NSSymbol symbol = NULL;
	// Allocate a buffer for the name, an underscore prefix, and a cstring terminator.
	std::string mangledName = "_" + procName;
	if (NSIsSymbolNameDefined(mangledName.c_str()))
		symbol = NSLookupAndBindSymbol(mangledName.c_str());
	return symbol ? NSAddressOfSymbol(symbol) : NULL;
}

bool CNSOGLManager::swapBuffers()
{
	[(NSOpenGLContext *)CurrentContext.OpenGLOSX.Context flushBuffer];

	return true;
}

}
}

#endif
