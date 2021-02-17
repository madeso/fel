add_subdirectory(fel/src)
add_subdirectory(lsp/src)

##############################################################################
## fel (unit) tests

add_executable(tests
    fel/src/fel/lexer.test.cc
    lsp/src/lsp/lsp.test.cc
)
target_link_libraries(
    tests
    PUBLIC
    fel
    lsp
    catchy_main
    fmt
    ${CMAKE_THREAD_LIBS_INIT}
    PRIVATE
    project_options
    project_warnings
)
catch_discover_tests(tests)
