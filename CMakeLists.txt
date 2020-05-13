cmake_minimum_required(VERSION 3.12.4)
project(fel CXX)


##############################################################################
## project setup
include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Sanitizers.cmake)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)
enable_sanitizers(project_options)

add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)


##############################################################################
## rapidjson dependency

add_library(rapidjson INTERFACE)
target_compile_definitions(rapidjson
  INTERFACE
  RAPIDJSON_HAS_STDSTRING=1
)
target_include_directories(rapidjson
    INTERFACE
    external/rapidjson/
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
)
target_include_directories(lsp
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(lsp
    PUBLIC
    rapidjson
    PRIVATE
    project_options
    project_warnings
)



##############################################################################
## logger library
find_package(Threads REQUIRED)

add_library(logger STATIC
    logger/logger.cc logger/logger.h
)
target_include_directories(logger
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
    external/spdlog/
)
target_link_libraries(logger
    PUBLIC
    ${CMAKE_THREAD_LIBS_INIT}
    PRIVATE
    project_options
    # project_warnings
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
    logger
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

