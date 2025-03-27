add_executable(inlab09 src/inlab09.cpp)
target_include_directories(inlab09 PRIVATE ./include)
add_custom_command(TARGET inlab09
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:inlab09> ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(inlab09 PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)
set_property(TARGET inlab09 PROPERTY OUTPUT_NAME shumilov_inlab09)

install(TARGETS inlab09 DESTINATION bin OPTIONAL)

add_executable(outlab09 src/outlab09.cpp)
target_include_directories(outlab09 PRIVATE ./include)
add_custom_command(TARGET outlab09
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:outlab09> ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(outlab09 PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)
set_property(TARGET outlab09 PROPERTY OUTPUT_NAME shumilov_outlab09)

install(TARGETS outlab09 DESTINATION bin OPTIONAL)

add_executable(test09 src/test.cpp)
target_include_directories(test09 PRIVATE ./include)
target_link_libraries(test09 PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)
