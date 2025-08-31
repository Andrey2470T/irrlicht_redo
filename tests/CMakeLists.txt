add_executable(IrrlichtRedoTest)

set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB_RECURSE TESTS_SOURCES ${TESTS_DIR}/*.cpp)
target_sources(IrrlichtRedoTest PRIVATE ${TESTS_SOURCES})

target_link_libraries(IrrlichtRedoTest PRIVATE ${PROJECT_NAME})
