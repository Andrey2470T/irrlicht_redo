# Compiler flags

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-DDEBUG)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

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
	add_definitions(-DWINDOWS)
elseif(APPLE)
	set(Platform "OSX")
	add_definitions(-DOSX)
elseif(ANDROID)
	set(Platform "ANDROID")
	add_definitions(-DANDROID)
elseif(EMSCRIPTEN)
	set(Platform "EMSCRIPTEN")
	add_definitions(-DEMSCRIPTEN)
elseif(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
	set(Platform "SOLARIS")
	add_definitions(-DSOLARIS)
else()
	set(Platform "LINUX")
	add_definitions(-DLINUX)
endif()

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()


# Joystick

if(NOT (BSD OR SOLARIS OR EMSCRIPTEN))
	add_definitions(-DCOMPILE_WITH_JOYSTICK_EVENTS)
endif()


# OpenGL

if(ANDROID)
	set(Renderer "OpenGLES2")
	add_definitions(-DOPENGL_ES_2)
elseif(EMSCRIPTEN)
	set(Renderer "WebGL1")
	add_definitions(-DWEBGL_1)
else()
	set(Renderer "OpenGL3+")
	add_definitions(-DOPENGL_3_PLUS)
endif()


# Misc

include(TestBigEndian)
TEST_BIG_ENDIAN(BIG_ENDIAN)
if(BIG_ENDIAN)
	add_definitions(-D__BIG_ENDIAN__)
endif()


# Print the current configuration
message(STATUS "Platform: ${Platform}")
message(STATUS "Renderer: ${Renderer}")


# Required libs

include(CheckCXXSourceCompiles)

find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

if(Renderer STREQUAL "OpenGLES2")
	find_package(OpenGLES2 REQUIRED)
elseif(Renderer STREQUAL "WebGL1")
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

# ...no target either.
if(NOT TARGET SDL2::SDL2)
	add_library(SDL2::SDL2 SHARED IMPORTED)
	set_target_properties(SDL2::SDL2 PROPERTIES
		IMPORTED_LOCATION "${SDL2_LIBRARIES}")
endif()


# Adding library

file(GLOB_RECURSE SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Image/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Main/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Render/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Utils/*.cpp
)

set(PRIVATE_HEADERS
	${ZLIB_INCLUDE_DIR}
	${JPEG_INCLUDE_DIR}
	${PNG_INCLUDE_DIR}
	${SDL2_INCLUDE_DIRS}
	${SDL2_IMAGE_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
	${OPENGLES2_INCLUDE_DIR}
	${WEBGL1_INCLUDE_DIR}
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME}
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE
		${PRIVATE_HEADERS}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		${ZLIB_LIBRARY}
		${JPEG_LIBRARY}
		${PNG_LIBRARY}
		GLEW::GLEW
		SDL2::Main
		${SDL2_IMAGE_LIBRARIES}
		# incl. transitive SDL2 dependencies for static linking
		"$<$<PLATFORM_ID:Android>:-landroid -llog -lGLESv2 -lGLESv1_CM -lOpenSLES>"

		"$<$<BOOL:${OPENGL_DIRECT_LINK}>:${OPENGL_LIBRARIES}>"
)
