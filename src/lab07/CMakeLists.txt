add_executable(lab07 ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

target_link_libraries(lab07 PRIVATE MPI::MPI_CXX
                                    fmt::fmt)

set_property(TARGET lab07 PROPERTY OUTPUT_NAME shumilov_inlab07)

add_custom_command(TARGET lab07
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:lab07> ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS lab07 DESTINATION bin OPTIONAL)
