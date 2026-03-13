# FindSDL2.cmake - кастомный модуль для поиска SDL2

# Если переменные уже заданы, используем их
if(SDL2_LIBRARY AND SDL2_INCLUDE_DIR)
    set(SDL2_LIBRARIES ${SDL2_LIBRARY})
    set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
    set(SDL2_FOUND TRUE)

    # Создаем target
    if(NOT TARGET SDL2::SDL2)
        add_library(SDL2::SDL2 UNKNOWN IMPORTED)
        set_target_properties(SDL2::SDL2 PROPERTIES
            IMPORTED_LOCATION "${SDL2_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIR}"
        )
    endif()

    message(STATUS "Found SDL2: ${SDL2_LIBRARY} (custom)")
    return()
endif()

# Если переменные не заданы, ищем стандартным способом
find_path(SDL2_INCLUDE_DIR NAMES SDL.h PATH_SUFFIXES SDL2)
find_library(SDL2_LIBRARY NAMES SDL2)

if(SDL2_INCLUDE_DIR AND SDL2_LIBRARY)
    set(SDL2_FOUND TRUE)
    set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
    set(SDL2_LIBRARIES ${SDL2_LIBRARY})

    if(NOT TARGET SDL2::SDL2)
        add_library(SDL2::SDL2 UNKNOWN IMPORTED)
        set_target_properties(SDL2::SDL2 PROPERTIES
            IMPORTED_LOCATION "${SDL2_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIR}"
        )
    endif()

    message(STATUS "Found SDL2: ${SDL2_LIBRARY}")
else()
    set(SDL2_FOUND FALSE)
    message(STATUS "SDL2 not found")
endif()
