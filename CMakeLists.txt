cmake_minimum_required(VERSION 3.6)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(fel)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# enable all warnings
if(MSVC)
  add_compile_options(/W4)
  add_compile_options(/GR-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
  add_compile_options(-Wshadow)
  add_compile_options(-fno-rtti)
  add_compile_options(-Wnon-virtual-dtor)
  # add_compile_options(-Wold-style-cast) # todo...
  add_compile_options(-Wcast-align)
  add_compile_options(-Wunused)
  add_compile_options(-Woverloaded-virtual)
  #add_compile_options(-Wconversion)
  #add_compile_options(-Wsign-conversion)
  add_compile_options(-Wformat=2)
  # add_compile_options(-Wlifetime)

  add_compile_options(-Wno-error=deprecated)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(src)

add_library(fel STATIC
  src/log.cc
  src/fel.cc
  src/value.cc
  src/location.cc
  src/file.cc
  src/lexer.cc
  )

add_executable(felapp
  cli/main.cc
  )
target_link_libraries(felapp fel)
set_property(TARGET felapp PROPERTY RUNTIME_OUTPUT_NAME fel)

include_directories(SYSTEM external/catch/)

add_executable(tests
  test/main.cc
  test/test.cc
  test/strings.cc
  test/comments.cc
  )
target_link_libraries(tests fel ${CMAKE_THREAD_LIBS_INIT})

