IrrlichtRedo 2.0-Release
======================

My fork of the IrrlichtMt [Minetest](https://github.com/minetest/lib/irr). Represents the fully rewritten and refactored version of the 3D engine which aiming to be more modern and readable. Many crusty stuff like the GUI, fixed-pipeline, scene management, manual loading of the OpenGL functions were removed, the part of which has been replaced to more lighweight and transparent abstractions like DrawContext, FrameBuffer, Shader and another part will be updated and re-added to the MT since the common goal of the library is to provide the low-level access to the OpenGL and SDL.

Features
-----

* Support of OpenGL 3.1+ and ES 2.0+ with using their new features.
* Low-level graphics API representing the abstraction wrappers over OpenGL.
* Much more flexible and readable comparing to the original, free from the the messed and invent-biked code.
* Support of SDL2_image for reading/writing images, SDL2_ttf for loading TrueType fonts.
* Doesn't expose the GUI, Mesh loaders and Scene interfaces anymore (later they will be integrated to the MT itself).

Build
-----

The build system is CMake.

The following libraries are required to be installed:
* libPNG
* libJPEG
* OpenGL
  * or on mobile: OpenGL ES (can be optionally enabled on desktop too)
* SDL2
* SDL2_image
* SDL2_ttf
* GLEW

However, IrrlichtMt cannot be built or installed separately.

Platforms
---------

* Windows via MinGW
* Linux (GL or GLES)
* macOS
* Android
* Solaris
* Emscripten

License
-------

The license of the Irrlicht Engine is based on the zlib/libpng license and applies to this fork, too.

	The Irrlicht Engine License
	===========================

	Copyright (C) 2002-2012 Nikolaus Gebhardt

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	 claim that you wrote the original software. If you use this software
	 in a product, an acknowledgement in the product documentation would be
	 appreciated but is not required.
	2. Altered source versions must be clearly marked as such, and must not be
	 misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
