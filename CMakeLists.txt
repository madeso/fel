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


add_subdirectory(external)
add_subdirectory(libs)
add_subdirectory(apps)
