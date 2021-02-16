add_library(json INTERFACE)
target_include_directories(json
    INTERFACE
    nlohmann_json/
)

add_subdirectory(catchy)

add_library(catchy_main STATIC main.catch.cc)
target_link_libraries(catchy_main PUBLIC catchy)
