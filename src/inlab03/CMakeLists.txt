add_executable(inlab03 main.cxx)
add_custom_command(TARGET inlab03
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:inlab03> ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(inlab03 PRIVATE methods ne591_compiler_flags fmt::fmt-header-only argparse)

set_property(TARGET inlab03 PROPERTY OUTPUT_NAME shumilov_inlab03)

install(TARGETS inlab03 DESTINATION bin OPTIONAL)