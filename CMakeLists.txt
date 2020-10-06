cmake_minimum_required(VERSION 3.12.4)
project(fel CXX)

set(CMAKE_CXX_EXTENSIONS OFF)

##############################################################################
## project setup
include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Sanitizers.cmake)
include(cmake/Defines.cmake)
include(ExternalProject)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)
enable_sanitizers(project_options)
set_project_defines(project_options)

add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)


##############################################################################
## json dependency

add_library(json INTERFACE)
target_include_directories(json
    INTERFACE
    external/nlohmann_json/
)


##############################################################################
## fel library

add_library(fel STATIC
    fel/file.cc fel/file.h
    fel/lexer.cc fel/lexer.h
    fel/location.cc fel/location.h
    fel/log.cc fel/log.h
)
target_include_directories(fel
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(fel
    PRIVATE
    project_options
    project_warnings
)



##############################################################################
## lsp library

add_library(lsp STATIC
    lsp/lsp.cc lsp/lsp.h
    lsp/str.cc lsp/str.h
)
target_include_directories(lsp
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(lsp
    PUBLIC
    json
    PRIVATE
    project_options
    project_warnings
)


##############################################################################
## fel commandline

add_executable(felapp
    cli/main.cc
)
target_link_libraries(felapp
    PUBLIC
    fel
    lsp
    PRIVATE
    project_options
    project_warnings
)
set_property(TARGET felapp PROPERTY RUNTIME_OUTPUT_NAME fel)



##############################################################################
## fel (unit) tests

add_executable(tests
    test/main.cc
    test/test_lexer.cc
    test/test_lsp.cc
    test/falsestring.cc test/falsestring.h
)
target_include_directories(tests
    PRIVATE
    external/catch/
)
target_link_libraries(
    tests
    PUBLIC
    fel
    lsp
    ${CMAKE_THREAD_LIBS_INIT}
    PRIVATE
    project_options
    project_warnings
)

