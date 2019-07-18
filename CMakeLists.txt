cmake_minimum_required(VERSION 3.6)
project(fel)

find_package(BISON)
find_package(FLEX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(src)

add_library(fel STATIC
  src/log.cc
  src/fel.cc
  )

add_executable(felapp
  cli/main.cc
  )
target_link_libraries(felapp fel)
set_property(TARGET felapp PROPERTY RUNTIME_OUTPUT_NAME fel)

include_directories(SYSTEM external/catch/)

add_executable(tests
  test/testmain.cc
  test/test.cc
  )

enable_testing()
add_test(tests tests)

target_link_libraries(tests fel ${CMAKE_THREAD_LIBS_INIT})

