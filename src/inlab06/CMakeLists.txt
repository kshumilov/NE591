add_executable(inlab06 main.cxx)
add_custom_command(TARGET inlab06
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:inlab06> ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(inlab06 PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)
set_property(TARGET inlab06 PROPERTY OUTPUT_NAME shumilov_inlab06)

install(TARGETS inlab06 DESTINATION bin OPTIONAL)