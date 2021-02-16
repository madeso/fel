add_library(lsp STATIC
    lsp/lsp.cc lsp/lsp.h
)
target_include_directories(lsp
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(lsp
    PUBLIC
    json
    PRIVATE
    fmt
    project_options
    project_warnings
)
