add_library(user_func SHARED user_func.cpp)

target_link_libraries(user_func PRIVATE ne591_compiler_flags)
target_include_directories(user_func
        INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)