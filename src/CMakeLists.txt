# Compiler flags

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions(_DEBUG_)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

if(APPLE)
	set(CMAKE_INSTALL_PREFIX "/Users/runner/work/irrlicht_redo/")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")

	add_compile_options(-Wall -pipe)

	# Enable SSE for floating point math on 32-bit x86 by default
	# reasoning see minetest issue #11810 and https://gcc.gnu.org/wiki/FloatingPointMath
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		include(CheckCXXSourceCompiles)
		check_cxx_source_compiles("#ifndef __i686__\n#error\n#endif\nint main(){}" IS_I686)
		if(IS_I686)
			message(STATUS "Detected Intel x86: using SSE instead of x87 FPU")
			add_compile_options(-mfpmath=sse -msse)
		endif()
	endif()

elseif(MSVC)
	string(APPEND CMAKE_CXX_STANDARD_LIBRARIES " msvcrt.lib")

	add_compile_options(/Zl)

	# Enable SSE for floating point math on 32-bit x86 by default
	# reasoning see minetest issue #11810 and https://gcc.gnu.org/wiki/FloatingPointMath
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		add_compile_options(/arch:SSE)
	endif()
endif()


# Platforms

if(WIN32)
	set(Platform "WINDOWS")
	add_compile_definitions(_WINDOWS_)
elseif(APPLE)
	set(Platform "OSX")
	add_compile_definitions(_OSX_)
elseif(ANDROID)
	set(Platform "ANDROID")
	add_compile_definitions(_ANDROID_)
elseif(EMSCRIPTEN)
	set(Platform "EMSCRIPTEN")
	add_compile_definitions(_EMSCRIPTEN_)
elseif(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
	set(Platform "SOLARIS")
	add_compile_definitions(_SOLARIS_)
else()
	set(Platform "LINUX")
	add_compile_definitions(_LINUX_)
endif()

if(MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS_)
endif()


# Joystick

# Note: Joysticks cause the stack smashing in core::MainWindow if SDL2 is initialized with them and none of them is open

option(ENABLE_JOYSTICKS "Init SDL2 with joystick events" TRUE)

if(NOT (BSD OR SOLARIS OR EMSCRIPTEN))
	add_compile_definitions(_COMPILE_WITH_JOYSTICK_EVENTS_)
endif()


# OpenGL

if(ANDROID)
	set(RENDERER "OpenGLES2")
elseif(EMSCRIPTEN)
	set(RENDERER "WebGL1")
else()
	set(RENDERER "OpenGL3+")
endif()

if(RENDERER STREQUAL "OpenGL3+")
	add_compile_definitions(_OPENGL_3_PLUS_)
elseif(RENDERER STREQUAL "OpenGLES2")
	add_compile_definitions(_OPENGL_ES_2_)
else()
	add_compile_definitions(_WEBGL_1_)
endif()

# Misc

include(TestBigEndian)
TEST_BIG_ENDIAN(BIG_ENDIAN)
if(BIG_ENDIAN)
	add_compile_definitions(__BIG_ENDIAN__)
endif()


# Print the current configuration
message(STATUS "Platform: ${Platform}")
message(STATUS "Renderer: ${RENDERER}")


# Required libs

include(CheckCXXSourceCompiles)

find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

if(RENDERER STREQUAL "OpenGLES2")
	find_package(OpenGLES2 REQUIRED)
elseif(RENDERER STREQUAL "WebGL1")
	find_package(WebGL1 REQUIRED)
else()
	find_package(OpenGL REQUIRED)
endif()

# unfortunately older SDL does not provide its version to cmake, so check header.
set(CMAKE_REQUIRED_INCLUDES ${SDL2_INCLUDE_DIRS})
unset(CHECK_SDL_VERSION CACHE)
check_cxx_source_compiles("#include <SDL_version.h>\n\
	#if !SDL_VERSION_ATLEAST(2, 0, 10)\n\
	#error\n\
	#endif\n\
	int main() {}" CHECK_SDL_VERSION)

if(NOT CHECK_SDL_VERSION)
	message(FATAL_ERROR "SDL2 is too old, required is at least 2.0.10!")
endif()


# Adding library

file(GLOB_RECURSE SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Image/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Render/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Utils/*.cpp
)

set(PRIVATE_HEADERS
	${JPEG_INCLUDE_DIR}
	${PNG_INCLUDE_DIR}
	${SDL2_INCLUDE_DIRS}
	${SDL2_IMAGE_INCLUDE_DIRS}
	${SDL2_TTF_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
	${OPENGLES2_INCLUDE_DIR}
	${WEBGL1_INCLUDE_DIR}
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

if(APPLE)
	install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(${PROJECT_NAME} PUBLIC _DEBUG_)
endif()

if(WIN32)
	target_compile_definitions(${PROJECT_NAME} PUBLIC _WINDOWS_)
elseif(APPLE)
	target_compile_definitions(${PROJECT_NAME} PUBLIC _OSX_)
elseif(ANDROID)
	target_compile_definitions(${PROJECT_NAME} PUBLIC _ANDROID_)
elseif(EMSCRIPTEN)
	target_compile_definitions(${PROJECT_NAME} PUBLIC _EMSCRIPTEN_)
elseif(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
	target_compile_definitions(${PROJECT_NAME} PUBLIC _SOLARIS_)
else()
	target_compile_definitions(${PROJECT_NAME} PUBLIC _LINUX_)
endif()

if(NOT (BSD OR SOLARIS OR EMSCRIPTEN))
	target_compile_definitions(${PROJECT_NAME} PUBLIC _COMPILE_WITH_JOYSTICK_EVENTS_)
endif()

if(RENDERER STREQUAL "OpenGL3+")
	target_compile_definitions(${PROJECT_NAME} PUBLIC _OPENGL_3_PLUS_)
elseif(RENDERER STREQUAL "OpenGLES2")
	target_compile_definitions(${PROJECT_NAME} PUBLIC _OPENGL_ES_2_)
else()
	target_compile_definitions(${PROJECT_NAME} PUBLIC _WEBGL_1_)
endif()

target_include_directories(${PROJECT_NAME}
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		${PRIVATE_HEADERS}
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
		${JPEG_LIBRARY}
		${PNG_LIBRARY}
		GLEW::GLEW
		${SDL2_LIBRARIES}
		${SDL2_IMAGE_LIBRARIES}
		${SDL2_TTF_LIBRARIES}
		# incl. transitive SDL2 dependencies for static linking
		"$<$<PLATFORM_ID:Android>:-landroid -llog -lGLESv2 -lGLESv1_CM -lOpenSLES>"

		"$<$<BOOL:${OPENGL_DIRECT_LINK}>:${OPENGL_LIBRARIES}>"
)
