cmake_minimum_required(VERSION 3.6)
project(fel)

find_package(BISON)
find_package(FLEX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

bison_target(MyParser src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cc)
flex_target(MyScanner src/lexer.l  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc COMPILE_FLAGS "--header-file=${CMAKE_CURRENT_BINARY_DIR}/lexer.hh")
add_flex_bison_dependency(MyScanner MyParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(src)

add_library(fel STATIC
  ${BISON_MyParser_OUTPUTS}
  ${FLEX_MyScanner_OUTPUTS}
  src/fel.cc
  )

add_executable(felapp
  src/main.cc
  )
target_link_libraries(felapp fel)

include_directories(SYSTEM external/catch/)

add_executable(tests
  src/testmain.cc
  src/test.cc
  )

enable_testing()
add_test(tests tests)

target_link_libraries(tests fel ${CMAKE_THREAD_LIBS_INIT})

