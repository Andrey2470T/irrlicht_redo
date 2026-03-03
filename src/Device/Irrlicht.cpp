// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "Device/CLogger.h"
static const char *const copyright = "Irrlicht Engine (c) 2002-2017 Nikolaus Gebhardt"; // put string in binary

#include "irrlicht.h"
#include "matrix4.h"
#include "SMaterial.h"
#include "CIrrDeviceSDL.h"

extern "C" IrrlichtDevice *createDeviceEx(const SIrrlichtCreationParameters &params)
{
	(void)copyright; // prevent unused variable warning

	IrrlichtDevice *dev = 0;

	if (params.DeviceType == EIDT_SDL || (!dev && params.DeviceType == EIDT_BEST))
		dev = new CIrrDeviceSDL(params);

	if (dev && !dev->getVideoDriver() && params.DriverType != video::EDT_NULL) {
		dev->closeDevice(); // destroy window
		dev->run();         // consume quit message
		dev->drop();
		dev = 0;
	}

	return dev;
}

extern "C" void showErrorMessageBox(IrrlichtDevice *dev,
	const char *title, const char *message)
{
	title = title ? title : "Irrlicht";
	bool ok = false;

	if (dev && dev->getType() == EIDT_SDL) {
		ok = static_cast<CIrrDeviceSDL*>(dev)->showErrorMessageBox(title, message);
	} else {
		ok = CIrrDeviceSDL::showErrorMessageBox(nullptr, title, message);
	}

	if (!ok) {
		g_irrlogger->log(title, message, ELL_ERROR);
	}
}

namespace core
{
const matrix4 IdentityMatrix(matrix4::EM4CONST_IDENTITY);
}
namespace video
{
SMaterial IdentityMaterial;
}

namespace video
{
extern "C" bool isDriverSupported(E_DRIVER_TYPE driver)
{
	switch (driver) {
	case EDT_NULL:
		return true;
#ifdef ENABLE_OPENGL3
	case EDT_OPENGL3:
		return true;
#endif
#ifdef _IRR_COMPILE_WITH_OGLES2_
	case EDT_OGLES2:
		return true;
#endif
#ifdef _IRR_COMPILE_WITH_WEBGL1_
	case EDT_WEBGL1:
		return true;
#endif
	default:
		return false;
	}
}
}
