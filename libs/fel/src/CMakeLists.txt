add_library(fel STATIC
    fel/file.cc fel/file.h
    fel/lexer.cc fel/lexer.h
    fel/location.cc fel/location.h
    fel/log.cc fel/log.h
    fel/ast.cc fel/ast.h
    fel/ast_printer.cc fel/ast_printer.h
    fel/object.cc fel/object.h
    fel/parser.cc fel/parser.h
    fel/where.cc fel/where.h
    fel/interpreter.cc fel/interpreter.h
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
