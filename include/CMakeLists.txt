add_library(methods INTERFACE)
target_include_directories(methods INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(methods INTERFACE ne591_compiler_flags fmt::fmt nlohmann_json::nlohmann_json )
