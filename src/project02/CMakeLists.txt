add_executable(project02 project02.cpp)
add_custom_command(TARGET project02
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:project02> ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(project02 PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)
set_property(TARGET project02 PROPERTY OUTPUT_NAME shumilov_project02)

install(TARGETS project02 DESTINATION bin OPTIONAL)