function(set_project_defines project_name)
    set(MSVC_DEFINES
        /D NOMINMAX
        /D STRICT
        /D WIN32_LEAN_AND_MEAN
        /D VC_EXTRALEAN
    )

    if(MSVC)
        set(PROJECT_DEFINES ${MSVC_DEFINES})
    else()
        set(PROJECT_DEFINES)
    endif()

    target_compile_definitions(${project_name} INTERFACE ${PROJECT_DEFINES})
endfunction()
