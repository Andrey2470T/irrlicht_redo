cmake_minimum_required(VERSION 3.12)

project(IrrlichtRedo
	VERSION 1.0.0
	LANGUAGES CXX
)

message(STATUS "*** Building IrrlichtRedo ${PROJECT_VERSION} ***")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type: Debug or Release" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
add_subdirectory(src)
