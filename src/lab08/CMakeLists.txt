add_executable(inlab08 ./src/inlab08.cpp)
target_link_libraries(inlab08 PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)
set_property(TARGET inlab08 PROPERTY OUTPUT_NAME shumilov_inlab08)
add_custom_command(TARGET inlab08
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:inlab08> ${CMAKE_CURRENT_SOURCE_DIR}
)
install(TARGETS inlab08 DESTINATION bin OPTIONAL)
