add_executable(inlab08 src/inlab08.cpp)
target_link_libraries(inlab08 PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json)
target_include_directories(inlab08 PRIVATE ./include)
set_property(TARGET inlab08 PROPERTY OUTPUT_NAME shumilov_inlab08)
add_custom_command(TARGET inlab08
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:inlab08> ${CMAKE_CURRENT_SOURCE_DIR}
)
install(TARGETS inlab08 DESTINATION bin OPTIONAL)

add_executable(outlab08 ./src/outlab08.cpp ./src/mpi_utils.cpp)
target_link_libraries(outlab08 PRIVATE methods ne591_compiler_flags fmt::fmt argparse nlohmann_json::nlohmann_json MPI::MPI_CXX)
target_include_directories(outlab08 PRIVATE ./include)
set_property(TARGET outlab08 PROPERTY OUTPUT_NAME shumilov_outlab08)
add_custom_command(TARGET outlab08
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:outlab08> ${CMAKE_CURRENT_SOURCE_DIR}
)
install(TARGETS outlab08 DESTINATION bin OPTIONAL)

add_executable(quad ./src/gauss_quad.cpp)
target_link_libraries(quad PRIVATE methods ne591_compiler_flags fmt::fmt argparse)
target_include_directories(quad PRIVATE ./include)
set_property(TARGET quad PROPERTY OUTPUT_NAME quad)
add_custom_command(TARGET quad
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:quad> ${CMAKE_CURRENT_SOURCE_DIR}
)
install(TARGETS quad DESTINATION bin OPTIONAL)
