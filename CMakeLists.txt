cmake_minimum_required(VERSION 3.15)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(fel CXX)

include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Sanitizers.cmake)

add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)

target_compile_features(project_options INTERFACE cxx_std_17)
set_project_warnings(project_warnings)
enable_sanitizers(project_options)

set(EXTRA_FLAGS "" CACHE STRING "extra flags to both compile and link")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EXTRA_FLAGS}")

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(src)

add_library(fel STATIC
    src/ast.cc
    src/ast.h
    src/bytecode.h
    src/bytecode.cc
    src/context.cc
    src/context.h
    src/file.cc
    src/file.h
    src/lexer.cc
    src/lexer.h
    src/location.cc
    src/location.h
    src/log.cc
    src/log.h
    src/parser.cc
    src/parser.h
)
target_link_libraries(fel
    PRIVATE project_options project_warnings
)

add_executable(felapp
    cli/main.cc
)
target_link_libraries(felapp
    PUBLIC fel
    PRIVATE project_options project_warnings
)
set_property(TARGET felapp PROPERTY RUNTIME_OUTPUT_NAME fel)

include_directories(SYSTEM external/catch/)

add_executable(tests
    test/main.cc
    test/test_bytecode.cc
    test/test_lexer.cc
    test/test_parser.cc
)
target_link_libraries(
    tests
    PUBLIC fel ${CMAKE_THREAD_LIBS_INIT}
    PRIVATE project_options project_warnings
)

