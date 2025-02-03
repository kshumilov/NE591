add_executable(inlab04 main.cxx)
add_custom_command(TARGET inlab04
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:inlab04> ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(inlab04 PRIVATE methods ne591_compiler_flags fmt::fmt argparse)

set_property(TARGET inlab04 PROPERTY OUTPUT_NAME shumilov_inlab04)

install(TARGETS inlab04 DESTINATION bin OPTIONAL)