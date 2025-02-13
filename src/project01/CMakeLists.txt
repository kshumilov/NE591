add_executable(project01 main.cxx)
add_custom_command(TARGET project01
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:project01> ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(project01 PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)
set_property(TARGET project01 PROPERTY OUTPUT_NAME shumilov_project01)

install(TARGETS project01 DESTINATION bin OPTIONAL)