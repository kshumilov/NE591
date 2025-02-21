add_executable(lab06 main.cxx)
target_link_libraries(lab06 PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)
set_property(TARGET lab06 PROPERTY OUTPUT_NAME shumilov_lab06)
add_custom_command(TARGET lab06
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:lab06> ${CMAKE_CURRENT_SOURCE_DIR}
)
install(TARGETS lab06 DESTINATION bin OPTIONAL)

add_executable(generate_matrix generate_matrix.cxx)
add_custom_command(TARGET generate_matrix
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:generate_matrix> ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(generate_matrix PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)
install(TARGETS generate_matrix DESTINATION bin OPTIONAL)

add_executable(compare_methods compare_methods.cxx)
add_custom_command(TARGET compare_methods
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:compare_methods> ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(compare_methods PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)
install(TARGETS compare_methods DESTINATION bin OPTIONAL)
