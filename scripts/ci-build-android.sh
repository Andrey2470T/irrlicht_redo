#!/bin/bash -e

# NOTE: this code is mostly copied from minetest_android_deps
# <https://github.com/minetest/minetest_android_deps>

png_ver=1.6.40
jpeg_ver=3.0.1
sdl2_ver=2.32.10
glew_ver=2.2.0

download () {
	get_tar_archive libpng "https://download.sourceforge.net/libpng/libpng-${png_ver}.tar.gz"
	get_tar_archive libjpeg "https://download.sourceforge.net/libjpeg-turbo/libjpeg-turbo-${jpeg_ver}.tar.gz"
	get_tar_archive libsdl2 "https://github.com/libsdl-org/SDL/releases/download/release-${sdl2_ver}/SDL2-${sdl2_ver}.tar.gz"
	get_tar_archive libglew "https://github.com/nigels-com/glew/releases/download/glew-${glew_ver}/glew-${glew_ver}.tgz"
}

build_png () {
	mkdir -p libpng
	pushd libpng
	$srcdir/libpng/configure --host=$CROSS_PREFIX
	make && make DESTDIR=$PWD install
	popd
}

build_jpeg () {
	mkdir -p libjpeg
	pushd libjpeg
	cmake $srcdir/libjpeg "${CMAKE_FLAGS[@]}" -DENABLE_SHARED=OFF
	make && make DESTDIR=$PWD install
	popd
}

build_sdl2 () {
	mkdir -p libsdl2
    pushd libsdl2
	cmake $srcdir/libsdl2 "${CMAKE_FLAGS[@]}" \
        -DSDL_STATIC=ON \
        -DSDL_SHARED=OFF \
		-DSDL_TEST=OFF \
		-DSDL2_DISABLE_SDL2MAIN=ON \
		-DCMAKE_INSTALL_PREFIX=$PWD/install
    make && make install
    popd
    
    echo "=== SDL2 installation structure ==="
    find $PWD/libsdl2/install -type f -name "*.h" | head -10
    echo "==================================="
}

# GLEW build function for Android
# GLEW build function for Android - ИСПРАВЛЕННАЯ (только статическая)
build_glew () {
	echo "Building GLEW $glew_ver for Android ($TARGET_ABI)..."
	
	mkdir -p libglew
	pushd libglew
	
	# Copy source to build directory
	cp -r $srcdir/libglew/* .
	
	# IMPORTANT: Clean any previous builds
	make clean || true
	
	# Создаем директорию для статической библиотеки
	mkdir -p lib
	
	# Компилируем объектный файл для статической библиотеки
	echo "Compiling GLEW static library..."
	${CC} -DGLEW_NO_GLU -DGLEW_STATIC ${CFLAGS} -fPIC -Iinclude -c src/glew.c -o glew.o
	
	# Создаем статическую библиотеку
	${AR} rcs lib/libGLEW.a glew.o
	
	# Create installation directories
	mkdir -p install/lib install/include/GL
	
	# Copy built files
	if [ -f lib/libGLEW.a ]; then
		cp lib/libGLEW.a install/lib/
		cp include/GL/glew.h install/include/GL/
		echo "GLEW build completed successfully"
	else
		echo "ERROR: GLEW build failed - libGLEW.a not found"
		exit 1
	fi
	
	popd
}

build () {
	# Build libraries
	build_png
	build_jpeg
	build_sdl2
	build_glew

	local libpng=$PWD/libpng/usr/local/lib/libpng.a
	local libjpeg=$(echo $PWD/libjpeg/opt/libjpeg-turbo/lib*/libjpeg.a)
	local libsdl2=$PWD/libsdl2/install/lib/libSDL2.a
	local libsdl2_include=$PWD/libsdl2/install/include/SDL2
	local libglew=$PWD/libglew/install/lib/libGLEW.a
	local libglew_include=$PWD/libglew/install/include

	# Создаем директорию для конфигурационных файлов GLEW
	mkdir -p $PWD/cmake/Modules

	# Создаем FindGLEW.cmake для Android
	cat > $PWD/cmake/Modules/FindGLEW.cmake << 'EOF'
	# Кастомный FindGLEW.cmake для Android
	# Всегда использует заранее определенные пути

	if(GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
		set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
		set(GLEW_LIBRARIES ${GLEW_LIBRARY})
		set(GLEW_FOUND TRUE)
    
		if(NOT TARGET GLEW::GLEW)
			add_library(GLEW::GLEW UNKNOWN IMPORTED)
			set_target_properties(GLEW::GLEW PROPERTIES
				IMPORTED_LOCATION "${GLEW_LIBRARY}"
				INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIR}"
			)
		endif()
    
		message(STATUS "Found GLEW (custom): ${GLEW_LIBRARY}")
		return()
	endif()

	# Если переменные не заданы, ищем стандартным способом
	find_path(GLEW_INCLUDE_DIR GL/glew.h)
	find_library(GLEW_LIBRARY NAMES GLEW glew32 libGLEW)

	if(GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
		set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
		set(GLEW_LIBRARIES ${GLEW_LIBRARY})
		set(GLEW_FOUND TRUE)
    
		if(NOT TARGET GLEW::GLEW)
			add_library(GLEW::GLEW UNKNOWN IMPORTED)
			set_target_properties(GLEW::GLEW PROPERTIES
				IMPORTED_LOCATION "${GLEW_LIBRARY}"
				INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIR}"
			)
		endif()
	endif()
EOF

	# Добавляем нашу директорию с модулями в путь поиска CMake
	CMAKE_FLAGS+=(-DCMAKE_MODULE_PATH="$PWD/cmake/Modules")

	# В вызове cmake добавляем явную передачу переменных GLEW
	cmake $srcdir/irrlicht "${CMAKE_FLAGS[@]}" \
		-DBUILD_SHARED_LIBS=OFF \
		-DPNG_LIBRARY=$libpng \
		-DPNG_PNG_INCLUDE_DIR=$(dirname "$libpng")/../include \
		-DJPEG_LIBRARY=$libjpeg \
		-DJPEG_INCLUDE_DIR=$(dirname "$libjpeg")/../include \
		-DSDL2_LIBRARIES=$libsdl2 \
		-DSDL2_INCLUDE_DIRS=$libsdl2_include \
		-DSDL2_LIBRARY=$libsdl2 \
		-DSDL2_INCLUDE_DIR=$libsdl2_include \
		-DGLEW_LIBRARY=$libglew \
		-DGLEW_INCLUDE_DIR=$libglew_include \
		-DGLEW_LIBRARIES=$libglew \
		-DGLEW_INCLUDE_DIRS=$libglew_include \
		-DCMAKE_CXX_FLAGS="-I${libglew_include} -DGLEW_NO_GLU ${CMAKE_CXX_FLAGS} -Wno-error -Wno-format-security" \
		-DCMAKE_C_FLAGS="-I${libglew_include} -DGLEW_NO_GLU ${CMAKE_C_FLAGS} -Wno-error -Wno-format-security"
		
	make
	
	echo "Looking for libIrrlichtRedo.a..."
	find . -name "libIrrlichtRedo.a" -type f

	if [ -f "lib/Android/libIrrlichtRedo.a" ]; then
		cp -p lib/Android/libIrrlichtRedo.a $pkgdir/
	elif [ -f "lib/libIrrlichtRedo.a" ]; then
		cp -p lib/libIrrlichtRedo.a $pkgdir/
	elif [ -f "src/libIrrlichtRedo.a" ]; then
		cp -p src/libIrrlichtRedo.a $pkgdir/
	else
		echo "ERROR: Cannot find libIrrlichtRedo.a"
		find . -name "*.a" -type f | head -20
		exit 1
	fi

	cp -p $libpng $libjpeg $libsdl2 $libglew $pkgdir/
	cp -a $srcdir/irrlicht/include $pkgdir/include
}

get_tar_archive () {
	# $1: folder to extract to, $2: URL
	local filename="${2##*/}"
	[ -d "$1" ] && return 0
	wget -c "$2" -O "$filename"
	mkdir -p "$1"
	tar -xaf "$filename" -C "$1" --strip-components=1
	rm "$filename"
}

_setup_toolchain () {
	local toolchain=$(echo "$ANDROID_NDK"/toolchains/llvm/prebuilt/*)
	if [ ! -d "$toolchain" ]; then
		echo "Android NDK path not specified or incorrect"; return 1
	fi
	export PATH="$toolchain/bin:$ANDROID_NDK:$PATH"

	unset CFLAGS CPPFLAGS CXXFLAGS

	TARGET_ABI="$1"
	API=21
	if [ "$TARGET_ABI" == armeabi-v7a ]; then
		CROSS_PREFIX=armv7a-linux-androideabi
		CFLAGS="-mthumb"
		CXXFLAGS="-mthumb"
	elif [ "$TARGET_ABI" == arm64-v8a ]; then
		CROSS_PREFIX=aarch64-linux-android
	elif [ "$TARGET_ABI" == x86 ]; then
		CROSS_PREFIX=i686-linux-android
		CFLAGS="-mssse3 -mfpmath=sse"
		CXXFLAGS="-mssse3 -mfpmath=sse"
	elif [ "$TARGET_ABI" == x86_64 ]; then
		CROSS_PREFIX=x86_64-linux-android
	else
		echo "Invalid ABI given"; return 1
	fi
	export CC=$CROSS_PREFIX$API-clang
	export CXX=$CROSS_PREFIX$API-clang++
	export AR=llvm-ar
	export RANLIB=llvm-ranlib
	export CFLAGS="-fPIC ${CFLAGS}"
	export CXXFLAGS="-fPIC ${CXXFLAGS}"

	CMAKE_FLAGS=(
		"-DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake"
		"-DANDROID_ABI=$TARGET_ABI" "-DANDROID_NATIVE_API_LEVEL=$API"
		"-DCMAKE_BUILD_TYPE=Release"
	)

	# make sure pkg-config doesn't interfere
	export PKG_CONFIG=/bin/false

	export MAKEFLAGS="-j$(nproc)"
}

_run_build () {
	local abi=$1
	irrdir=$PWD

	mkdir -p $RUNNER_TEMP/src
	cd $RUNNER_TEMP/src
	srcdir=$PWD
	[ -d irrlicht ] || ln -s $irrdir irrlicht
	download

	builddir=$RUNNER_TEMP/build/irrlicht-$abi
	pkgdir=$RUNNER_TEMP/pkg/$abi/Irrlicht
	rm -rf "$pkgdir"
	mkdir -p "$builddir" "$pkgdir"

	cd "$builddir"
	build
}

if [ $# -lt 1 ]; then
	echo "Usage: ci-build-android.sh <ABI>"
	exit 1
fi

_setup_toolchain $1
_run_build $1
