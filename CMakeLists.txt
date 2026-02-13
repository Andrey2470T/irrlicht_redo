cmake_minimum_required(VERSION 3.12)

project(IrrlichtRedo
	VERSION 1.0.0
	LANGUAGES CXX
)

message(STATUS "*** Building IrrlichtRedo ${PROJECT_VERSION} ***")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

if(ANDROID)
	set(sysname Android)
elseif(APPLE)
	set(sysname OSX)
elseif(MSVC)
	set(sysname Win32-VisualStudio)
elseif(WIN32)
	set(sysname Win32-gcc)
else()
	set(sysname Linux)
endif()
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib/${sysname})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/${sysname})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type: Debug or Release" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
enable_testing()
add_subdirectory(src)
add_subdirectory(test)

# Export a file that describes the targets that IrrlichtRedo creates.
# The file is placed in the location FILE points to, where CMake can easily
# locate it by pointing CMAKE_PREFIX_PATH to this project root.
export(EXPORT IrrlichtRedo-export
	FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/IrrlichtRedoTargets.cmake"
	NAMESPACE IrrlichtRedo::
)

# Installation of headers.
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/irrlicht_redo"
)

# Installation of CMake target and configuration files.
install(EXPORT IrrlichtRedo-export
	FILE IrrlichtRedoTargets.cmake
	NAMESPACE IrrlichtRedo::
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IrrlichtRedo"
)

include(CMakePackageConfigHelpers)
configure_package_config_file("${PROJECT_SOURCE_DIR}/Config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/IrrlichtRedoConfig.cmake"
	INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IrrlichtRedo"
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/IrrlichtRedoConfigVersion.cmake"
	COMPATIBILITY AnyNewerVersion
)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/IrrlichtRedoConfig.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/IrrlichtRedoConfigVersion.cmake"
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IrrlichtRedo"
)
