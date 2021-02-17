add_executable(felapp
    main.cc
)
target_link_libraries(felapp
    PUBLIC
    fel
    lsp
    fmt::fmt
    PRIVATE
    project_options
    project_warnings
)
set_property(TARGET felapp PROPERTY RUNTIME_OUTPUT_NAME fel)
