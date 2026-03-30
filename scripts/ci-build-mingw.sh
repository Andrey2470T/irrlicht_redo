#!/bin/bash -e
topdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

[[ -z "$CC" || -z "$CXX" ]] && exit 255
variant=win32
[[ "$(basename "$CXX")" == "x86_64-"* ]] && variant=win64

libjpeg_version=3.0.1
libpng_version=1.6.40
sdl2_version=2.28.5
zlib_version=1.3.1
glew_version=2.2.0

download () {
	local url=$1
	local filename=${url##*/}
	local foldername=${filename%%[.-]*}

	[ -d "./$foldername" ] && return 0
    [ -e "$filename" ] || wget "$url" -O "$filename"
	sha256sum -w -c <(grep -F "$filename" "$topdir/sha256sums.txt")
	unzip -o "$filename" -d "$foldername"
}

# As the sfan5's win64 libraries builds don`t have GLEW, we build it manually
download_glew () {
	echo "Downloading GLEW $glew_version for $variant..."

	glew_url="https://sourceforge.net/projects/glew/files/glew/${glew_version}/glew-${glew_version}-win32.zip/download"
	glew_zip="glew-${glew_version}-win32.zip"
	
	if [ ! -f "$glew_zip" ]; then
		wget "$glew_url" -O "$glew_zip"
	fi

	if grep -q "$glew_zip" "$topdir/sha256sums.txt" 2>/dev/null; then
		sha256sum -w -c <(grep -F "$glew_zip" "$topdir/sha256sums.txt")
	fi
	
	rm -rf glew-temp
	unzip -q "$glew_zip" -d glew-temp
	
	mkdir -p glew/{lib,include/GL,bin}
	
	cp -r glew-temp/glew-${glew_version}/include/GL/* glew/include/GL/
	
	if [ "$variant" = "win64" ]; then
		# For 64-bit
		cp glew-temp/glew-${glew_version}/lib/Release/x64/* glew/lib/ 2>/dev/null || true
		cp glew-temp/glew-${glew_version}/bin/Release/x64/* glew/bin/ 2>/dev/null || true
	else
		# For 32-bit
		cp glew-temp/glew-${glew_version}/lib/Release/Win32/* glew/lib/ 2>/dev/null || true
		cp glew-temp/glew-${glew_version}/bin/Release/Win32/* glew/bin/ 2>/dev/null || true
	fi
	
	if [ -f "glew/lib/glew32.lib" ]; then
		cp glew/lib/glew32.lib glew/lib/libGLEW.a 2>/dev/null || true
		cp glew/lib/glew32.lib glew/lib/libGLEW.dll.a 2>/dev/null || true
		cp glew/lib/glew32.lib glew/lib/libGLEW.so 2>/dev/null || true
	fi
	# ====================================================
	
	rm -rf glew-temp
	
	echo "GLEW download completed"
}

libs=$PWD/libs
mkdir -p libs
pushd libs
libhost="http://minetest.kitsunemimi.pw"

download "$libhost/llvm/libjpeg-$libjpeg_version-$variant.zip"
download "$libhost/llvm/libpng-$libpng_version-$variant.zip"
download "$libhost/llvm/sdl2-$sdl2_version-$variant.zip"
download "$libhost/llvm/zlib-$zlib_version-$variant.zip"

download_glew

popd

glew_include_dir="$libs/glew/include"

if [ -f "$libs/glew/lib/libGLEW.a" ]; then
	glew_library="$libs/glew/lib/libGLEW.a"
elif [ -f "$libs/glew/lib/libglew32.a" ]; then
	glew_library="$libs/glew/lib/libglew32.a"
elif [ -f "$libs/glew/lib/glew32.lib" ]; then
	glew_library="$libs/glew/lib/glew32.lib"
else
	echo "ERROR: Cannot find GLEW library"
	exit 1
fi

glew_dll=$(find $libs/glew/bin -name "*.dll" 2>/dev/null | head -1)
# ============================================================

cmake . \
	-DCMAKE_SYSTEM_NAME=Windows \
	-DPNG_LIBRARY="$libs/libpng/lib/libpng.dll.a" \
	-DPNG_PNG_INCLUDE_DIR="$libs/libpng/include" \
	-DJPEG_LIBRARY="$libs/libjpeg/lib/libjpeg.dll.a" \
	-DJPEG_INCLUDE_DIR="$libs/libjpeg/include" \
	-DZLIB_LIBRARY="$libs/zlib/lib/libz.dll.a" \
	-DZLIB_INCLUDE_DIR="$libs/zlib/include" \
	-DCMAKE_PREFIX_PATH="$libs/sdl2/lib/cmake" \
	-DSDL2_LIBRARIES="$libs/sdl2/lib/libSDL2.dll.a" \
	-DSDL2_INCLUDE_DIRS="$libs/sdl2/include/SDL2" \
	-DSDL2_LIBRARY="$libs/sdl2/lib/libSDL2.dll.a" \
	-DSDL2_INCLUDE_DIR="$libs/sdl2/include/SDL2" \
	-DDISABLE_CHECK_SDL_VERSION=TRUE \
	-DSDL2_VERSION=2.28.5 \
	-DGLEW_LIBRARY="$glew_library" \
	-DGLEW_INCLUDE_DIR="$glew_include_dir" \
	-DCMAKE_CXX_FLAGS="-I${glew_include_dir}" \
    -DCMAKE_C_FLAGS="-I${glew_include_dir}"

echo "=== Build configuration ==="
echo "Using GLEW library: $glew_library"
echo "Using GLEW include: $glew_include_dir"
echo "GLEW library exists: $(ls -la "$glew_library" 2>/dev/null || echo 'NOT FOUND')"
echo "GLEW header exists: $(ls -la "$glew_include_dir/GL/glew.h" 2>/dev/null || echo 'NOT FOUND')"
if [ -n "$glew_dll" ]; then
	echo "GLEW DLL exists: $(ls -la "$glew_dll" 2>/dev/null || echo 'NOT FOUND')"
fi
echo "=========================="

if [ ! -f "$glew_library" ]; then
	echo "ERROR: GLEW library not found at $glew_library"
	exit 1
fi

if [ ! -f "$glew_include_dir/GL/glew.h" ]; then
	echo "ERROR: GLEW header not found at $glew_include_dir/GL/glew.h"
	exit 1
fi

make -j$(nproc)

if [ "$1" = "package" ]; then
    # Создаем директорию для установки вручную
    mkdir -p _install/usr/local/lib
    mkdir -p _install/usr/local/include
    
    # Копируем собранную библиотеку
    find . -name "*.a" -type f -exec cp -p {} _install/usr/local/lib/ \;
    find . -name "*.dll" -type f -exec cp -p {} _install/usr/local/lib/ \;
    
    # Копируем заголовочные файлы
    cp -r include _install/usr/local/ 2>/dev/null || true
    
    # strip library
    if [ -f "_install/usr/local/lib/libIrrlichtRedo.a" ]; then
        "${CXX%-*}-strip" --strip-unneeded _install/usr/local/lib/libIrrlichtRedo.a 2>/dev/null || true
    fi
    
    # bundle the DLLs from various places
    shopt -s nullglob
    cp -p $libs/*/bin/*.dll _install/usr/local/lib/ 2>/dev/null || true
    cp -p $libs/glew/bin/*.dll _install/usr/local/lib/ 2>/dev/null || true
    
    # create a ZIP
    (cd _install/usr/local; zip -9r "$OLDPWD/irrlicht-$variant$extras.zip" -- *)
fi
exit 0
