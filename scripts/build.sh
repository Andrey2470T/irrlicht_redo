#!/bin/bash -e

build_on_linux() {
	cmake -B build \
		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE:-Debug} \
		${CMAKE_FLAGS}

	cmake --build build --parallel $(($(nproc) + 1))
}

build_on_macos() {
	mkdir build
	cd build
	cmake .. \
		-DCMAKE_OSX_DEPLOYMENT_TARGET=$1 \
		-DCMAKE_FIND_FRAMEWORK=LAST \
		-DCMAKE_INSTALL_PREFIX=../build/macos/
	cmake --build . -j$(sysctl -n hw.logicalcpu)
	make install
}
