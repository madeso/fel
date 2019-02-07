cmake_minimum_required(VERSION 3.6)
project(fel)

find_package(BISON)
find_package(FLEX)

bison_target(MyParser src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cc COMPILE_FLAGS "--defines=${CMAKE_CURRENT_BINARY_DIR}/parser_defines.hh")
flex_target(MyScanner src/lexer.l  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc COMPILE_FLAGS "--header-file=${CMAKE_CURRENT_BINARY_DIR}/lexer.hh")
add_flex_bison_dependency(MyScanner MyParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(src)

add_executable(fel
  ${BISON_MyParser_OUTPUTS}
  ${FLEX_MyScanner_OUTPUTS}
  src/fel.cc
  src/main.cc
  )
