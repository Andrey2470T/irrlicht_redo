set(DEFAULT_SDL3 OFF)

include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)

option(USE_SDL2_STATIC "Link with SDL2 static libraries" FALSE)

option(USE_SDL3 "Use the SDL3 backend instead of SDL3" ${DEFAULT_SDL3})
option(USE_SDL3_STATIC "Link with SDL3 static libraries" FALSE)

# Compiler flags

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions(_DEBUG)
endif()
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")

	add_compile_options(-Wall -pipe)

	# Enable SSE for floating point math on 32-bit x86 by default
	# reasoning see minetest issue #11810 and https://gcc.gnu.org/wiki/FloatingPointMath
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		check_cxx_source_compiles("#ifndef __i686__\n#error\n#endif\nint main(){}" IS_I686)
		if(IS_I686)
			message(STATUS "Detected Intel x86: using SSE instead of x87 FPU")
			add_compile_options(-mfpmath=sse -msse)
		endif()
	endif()

elseif(MSVC)
	string(APPEND CMAKE_CXX_STANDARD_LIBRARIES " msvcrt.lib") # ???? fuck off

	add_compile_definitions(
		# Suppress some useless warnings
		_CRT_SECURE_NO_DEPRECATE
		_CRT_SECURE_NO_WARNINGS
		# Get M_PI to work
		_USE_MATH_DEFINES
		# Don't define min/max macros in minwindef.h
		NOMINMAX
	)

	add_compile_options(/Zl)

	# Enable SSE for floating point math on 32-bit x86 by default
	# reasoning see minetest issue #11810 and https://gcc.gnu.org/wiki/FloatingPointMath
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		add_compile_options(/arch:SSE)
	endif()
endif()

# Platform-specific configuration

if(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
	set(SOLARIS TRUE)
endif()

# Platform (necessary for CFileSystem at least)

if(WIN32)
	add_compile_definitions(_IRR_WINDOWS_ _IRR_WINDOWS_API_)
elseif(APPLE)
	add_compile_definitions(_IRR_OSX_PLATFORM_)
elseif(ANDROID)
	add_compile_definitions(_IRR_ANDROID_PLATFORM_)
elseif(EMSCRIPTEN)
	add_compile_definitions(_IRR_EMSCRIPTEN_PLATFORM_)
	set(LINUX_PLATFORM TRUE)
elseif(SOLARIS)
	add_compile_definitions(_IRR_SOLARIS_PLATFORM_ _IRR_POSIX_API_)
else()
	add_compile_definitions(_IRR_POSIX_API_)
	set(LINUX_PLATFORM TRUE)
endif()

if(LINUX_PLATFORM)
	add_compile_definitions(_IRR_LINUX_PLATFORM_)
endif()

# Joystick

if(NOT (BSD OR SOLARIS OR EMSCRIPTEN))
	add_compile_definitions(_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
endif()

# OpenGL (Desktop, ES and Web)

option(ENABLE_OPENGL3 "Enable OpenGL 3+" TRUE)
option(ENABLE_GLES2 "Enable OpenGL ES 2+" FALSE)
set(ENABLE_WEBGL1 FALSE)
set(RENDERER "OpenGL 3+")

if(ANDROID OR EMSCRIPTEN)
	set(ENABLE_OPENGL3 FALSE)
	set(ENABLE_GLES2 TRUE)

	if(EMSCRIPTEN)
		set(ENABLE_WEBGL1 TRUE)
	endif()
endif()

if(ENABLE_GLES2)
	set(RENDERER "OpenGL ES 2+")

	add_compile_definitions(_IRR_COMPILE_WITH_OGLES2_)

	if(ENABLE_WEBGL1)
		set(RENDERER "WebGL 1")

		add_compile_definitions(_IRR_COMPILE_WITH_WEBGL1_)
	endif()
else()
	add_compile_definitions(_IRR_COMPILE_WITH_OPENGL3_)
	set(OPENGL_DIRECT_LINK TRUE) # driver relies on this
endif()


# Misc

include(TestBigEndian)
TEST_BIG_ENDIAN(BIG_ENDIAN)
if(BIG_ENDIAN)
	add_compile_definitions(__BIG_ENDIAN__)
endif()

# Configuration report

message(STATUS "Renderer: ${RENDERER}")

# Required libs

include(CheckCXXSourceCompiles)

if(ANDROID_ABI)
	if (CUSTOM_GLEW_DIR)
		message(STATUS "Using custom GLEW (${CUSTOM_GLEW_DIR}) for Android ABI: ${ANDROID_ABI}")

		set(GLEW_FOUND TRUE)
		set(GLEW_INCLUDE_DIR "${CUSTOM_GLEW_DIR}/${ANDROID_ABI}/glew/include")
		set(GLEW_LIBRARY "${CUSTOM_GLEW_DIR}/${ANDROID_ABI}/glew/lib/libGLEW.a")
		set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
		set(GLEW_LIBRARIES ${GLEW_LIBRARY})
		set(CMAKE_CXX_FLAGS "-I${GLEW_INCLUDE_DIR} -DGLEW_NO_GLU ${CMAKE_CXX_FLAGS} -Wno-error -Wno-format-security")
		set(CMAKE_C_FLAGS "-I${GLEW_INCLUDE_DIR} -DGLEW_NO_GLU ${CMAKE_C_FLAGS} -Wno-error -Wno-format-security")
	else()
		message(FATAL_ERROR "CUSTOM_GLEW_DIR (${CUSTOM_GLEW_DIR}) is not set for Android ABI: ${ANDROID_ABI}")
	endif()
else()
	find_package(GLEW REQUIRED)
endif()

# Create GLEW::GLEW target if not provided by find_package
if(NOT TARGET GLEW::GLEW)
	if(GLEW_LIBRARY AND GLEW_INCLUDE_DIR)
		add_library(GLEW::GLEW UNKNOWN IMPORTED)
		set_target_properties(GLEW::GLEW PROPERTIES
			IMPORTED_LOCATION "${GLEW_LIBRARY}"
			INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIR}")
	else()
		message(FATAL_ERROR "GLEW found but GLEW::GLEW target not available and GLEW_LIBRARY/GLEW_INCLUDE_DIR not set")
	endif()
endif()

find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

if(ENABLE_OPENGL3)
	find_package(OpenGL REQUIRED)
else()
	find_package(OpenGLES2 REQUIRED)
endif()

set(USE_SDL2_SHARED FALSE)

if(USE_SDL3)
	set(USE_SDL2_STATIC FALSE) # do not attempt to link against SDL2

	# https://wiki.libsdl.org/SDL3/README-cmake#a-system-sdl-library
	if(USE_SDL3_STATIC)
		set(SDL3_SUFFIX "static")
	else()
		set(SDL3_SUFFIX "shared")
	endif()
	find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-${SDL3_SUFFIX})
	message(STATUS "Found SDL3: ${SDL3_VERSION} (${SDL3_SUFFIX} library)")
else()
	# Fall back to SDL2

	if(NOT USE_SDL2_STATIC)
		set(USE_SDL2_SHARED TRUE)
	endif()
	if(NOT ANDROID)
		find_package(SDL2 REQUIRED)
	else()
		# provided by AndroidLibs.cmake
	endif()
	message(STATUS "Found SDL2: ${SDL2_LIBRARIES}")

	# unfortunately older SDL does not provide its version to cmake, so check header.
	set(CMAKE_REQUIRED_INCLUDES ${SDL2_INCLUDE_DIRS})

	if(NOT DISABLE_CHECK_SDL_VERSION)
		unset(CHECK_SDL_VERSION CACHE)
		check_cxx_source_compiles("#include <SDL_version.h>\n\
#if !SDL_VERSION_ATLEAST(2, 0, 10)\n\
#error\n\
#endif\n\
int main() {}" CHECK_SDL_VERSION)
		if(NOT CHECK_SDL_VERSION)
			message(FATAL_ERROR "SDL2 is too old, required is at least 2.0.10!")
		endif()
	endif()

	# ...no target either.
	if(NOT TARGET SDL2::SDL2)
		add_library(SDL2::SDL2 SHARED IMPORTED)
		set_target_properties(SDL2::SDL2 PROPERTIES
			IMPORTED_LOCATION "${SDL2_LIBRARIES}")
	endif()
endif()

if(APPLE)
	add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

set(link_includes
	"${PROJECT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_SOURCE_DIR}"

	"${ZLIB_INCLUDE_DIR}"
	"${JPEG_INCLUDE_DIR}"
	"${PNG_INCLUDE_DIR}"
	"$<$<NOT:$<BOOL:${USE_SDL3}>>:${SDL2_INCLUDE_DIRS}>"

	${OPENGL_INCLUDE_DIR}
	${OPENGLES2_INCLUDE_DIR}
)

set(sdl2_link_includes "$<$<NOT:$<BOOL:${USE_SDL3}>>:${SDL2_INCLUDE_DIRS}>")

# Source files

set(IRRMESHLOADER
	Mesh/CB3DMeshFileLoader.cpp
	Mesh/COBJMeshFileLoader.cpp
	Mesh/CXMeshFileLoader.cpp
)

add_library(IRRMESHOBJ OBJECT
	Mesh/SkinnedMesh.cpp
	Scene/CBoneSceneNode.cpp
	Scene/CMeshSceneNode.cpp
	Scene/CAnimatedMeshSceneNode.cpp
	${IRRMESHLOADER}
)

# the unified drivers

if(ENABLE_OPENGL3 OR ENABLE_GLES2)
	set(IRRDRVROBJ
		${IRRDRVROBJ}
		Video/DrawContext.cpp
		Video/GLSpecificInfo.cpp
		Video/FixedPipelineRenderer.cpp
		Video/MaterialRenderer.cpp
		Video/MaterialSystem.cpp
		Video/Renderer2D.cpp
		Video/RenderTarget.cpp
		Video/VBO.cpp
		Video/VideoDriver.cpp
	)
endif()

set(IRRIMAGEOBJ
	Image/CColorConverter.cpp
	Image/CImage.cpp
	Image/CImageLoaderJPG.cpp
	Image/CImageLoaderPNG.cpp
	Image/CImageLoaderTGA.cpp
	Image/CImageWriterJPG.cpp
	Image/CImageWriterPNG.cpp
)

add_library(IRRVIDEOOBJ OBJECT
	${IRRDRVROBJ}
	${IRRIMAGEOBJ}
)

add_library(IRRIOOBJ OBJECT
	IO/CFileList.cpp
	IO/CFileSystem.cpp
	IO/CLimitReadFile.cpp
	IO/CMemoryFile.cpp
	IO/CReadFile.cpp
	IO/CWriteFile.cpp
	IO/CZipReader.cpp
)

add_library(IRRDEVICEOBJ OBJECT
	Device/byteswap.cpp
	Device/SDLDevice.cpp
	Device/Clipboard.cpp
	Device/CursorControl.cpp
	Device/globals.cpp
	Device/Logger.cpp
	Device/Timer.cpp
)

if(ENABLE_OPENGL3)
	target_compile_definitions(IRRDEVICEOBJ PRIVATE ENABLE_OPENGL3)
endif()

if(APPLE)
	# Build all IRRDEVICEOBJ sources as objc++, including the .cpp's
	set_target_properties(IRRDEVICEOBJ PROPERTIES COMPILE_OPTIONS "-xobjective-c++")
endif()

if(USE_SDL3)
	include_directories(IRROTHEROBJ $<TARGET_PROPERTY:SDL3::Headers,INTERFACE_INCLUDE_DIRECTORIES>)
	target_compile_definitions(IRROTHEROBJ PUBLIC _IRR_USE_SDL3_)
	target_compile_definitions(IRRVIDEOOBJ PUBLIC _IRR_USE_SDL3_) # Video/Common.h
endif()

add_library(IRRGUIOBJ OBJECT
	GUI/CGUIButton.cpp
	GUI/CGUICheckBox.cpp
	GUI/CGUIComboBox.cpp
	GUI/CGUIEditBox.cpp
	GUI/CGUIEnvironment.cpp
	GUI/CGUIFileOpenDialog.cpp
	GUI/CGUIFont.cpp
	GUI/CGUIImage.cpp
	GUI/CGUIListBox.cpp
	GUI/CGUIScrollBar.cpp
	GUI/CGUISkin.cpp
	GUI/CGUIStaticText.cpp
	GUI/CGUITabControl.cpp
	GUI/CGUISpriteBank.cpp
	GUI/CGUIImageList.cpp
)

# Library

# There have to be some sources in IrrlichtRedo to workaround Cmake Xcode generator bug
add_library(IrrlichtRedo STATIC
	Scene/CBillboardSceneNode.cpp
	Scene/CCameraSceneNode.cpp
	Scene/CDummyTransformationSceneNode.cpp
	Scene/CEmptySceneNode.cpp
	Mesh/CMeshManipulator.cpp
	Scene/CSceneCollisionManager.cpp
	Scene/CSceneManager.cpp
	Mesh/CMeshCache.cpp
)

foreach(object_lib
	IRRMESHOBJ IRRVIDEOOBJ
	IRRIOOBJ IRRDEVICEOBJ IRRGUIOBJ)
	# Set include directories for object library compilation
	target_include_directories(${object_lib} PRIVATE ${link_includes})
	if(CMAKE_GENERATOR STREQUAL "Xcode")
		# Workaround for Cmake Xcode project generator
		target_link_libraries(IrrlichtRedo PRIVATE ${object_lib})
	else()
		# Add objects from object library to main library
		target_sources(IrrlichtRedo PRIVATE $<TARGET_OBJECTS:${object_lib}>)
	endif()

	if(BUILD_WITH_TRACY)
		target_link_libraries(${object_lib} PRIVATE Tracy::TracyClient)
	endif()
endforeach()

# Alias target provides add_submodule compatibility
add_library(IrrlichtRedo::IrrlichtRedo ALIAS IrrlichtRedo)

set_target_properties(IrrlichtRedo PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(IrrlichtRedo
	PUBLIC
		"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>"
		${sdl2_link_includes}
	PRIVATE
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
		${link_includes}
)

# this needs to be here and not in a variable (like link_includes) due to issues
# with the generator expressions on at least CMake 3.22, but not 3.28 or later
target_link_libraries(IrrlichtRedo PRIVATE
	${ZLIB_LIBRARY}
	${JPEG_LIBRARY}
	${PNG_LIBRARY}
	GLEW::GLEW

	"$<$<BOOL:${OPENGL_DIRECT_LINK}>:${OPENGL_LIBRARIES}>"

	# incl. transitive SDL2 dependencies for static linking
	"$<$<PLATFORM_ID:Android>:-landroid -llog -lGLESv2 -lGLESv1_CM -lOpenSLES>"
)

target_link_libraries(IrrlichtRedo PUBLIC
	"$<$<BOOL:${USE_SDL2_SHARED}>:SDL2::SDL2>"
	"$<$<BOOL:${USE_SDL2_STATIC}>:SDL2::SDL2-static>"

	"$<$<BOOL:${USE_SDL3}>:SDL3::SDL3-${SDL3_SUFFIX}>"
)

if(WIN32)
	target_compile_definitions(IrrlichtRedo INTERFACE _IRR_WINDOWS_API_)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(IrrlichtRedo INTERFACE _DEBUG)
endif()
if(APPLE OR ANDROID OR EMSCRIPTEN)
	target_compile_definitions(IrrlichtRedo PUBLIC IRR_MOBILE_PATHS)
endif()
