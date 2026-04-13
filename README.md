IrrlichtRedo version 1.2.0
==========================

My fork of the IrrlichtMt [Minetest](https://github.com/minetest/irrlicht).
Represents the much modified version of the 3D engine which aiming to be more modern and readable.

Features
-----

* Support of OpenGL 3.2+ and ES 2.0+ with using their new features.
* Using GLEW instead of the own opengl loader
* Port of the latest SDL2/SDL3 from luanti-org's irrlichtmt
* Low-level graphics API representing the abstraction wrappers over OpenGL.
* Free of the any platform-specific code like the timer, printing, clipboard, byteswapping and etc.

Build
-----

The build system is CMake.

The following libraries are required to be installed:
* GLEW 2.2.0+
* zlib, libPNG, libJPEG
* OpenGL 3.0+ (or OpenGL ES 2.0+ if on mobile)
* SDL2 or SDL3 (see below)

Aside from standard search options (`ZLIB_INCLUDE_DIR`, `ZLIB_LIBRARY`, ...) the following options are available:
* `ENABLE_OPENGL3` (default: `ON`) - Enable OpenGL 3+ driver
* `ENABLE_GLES2` - Enable OpenGL ES 2+ driver
* `USE_SDL2` (default: platform-dependent, usually `ON`) - Use SDL2
* `USE_SDL3` (default: `OFF`) - Use the SDL3 device instead of SDL2 (**experimental**)

e.g. on a Linux system you might want to build for local use like this:

	git clone https://github.com/Andrey2470T/irrlicht_redo
	cd irrlicht_redo
	cmake --build . --parallel $(nproc)

Platforms
---------

We aim to support these platforms:
* Windows via MinGW
* Linux (GL or GLES)
* macOS
* Android

This doesn't mean other platforms don't work or won't be supported, if you find something that doesn't work contributions are welcome.

Compatibility matrix
--------------------

Driver (rows) vs Device (columns)

|                           | SDL2 [1] | SDL3 [2] |
|---------------------------|----------|----------|
| OpenGL 3.2+               | Works    | Works    |
| OpenGL ES 2.x             | Works    | Testing  |
| WebGL 1                   | ?        | ?        |

Notes:

 * [1] `SDLDevice` with `USE_SDL3=0`: supports [many platforms](https://wiki.libsdl.org/SDL3/README-platforms)
 * [2] `SDLDevice` with `USE_SDL3=1`

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
