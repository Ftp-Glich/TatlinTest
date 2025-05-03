add_executable(tatlin_test main.cpp)

target_link_libraries(tatlin_test PRIVATE processer tape)

target_include_directories(tatlin_test PRIVATE 
    ${PROJECT_SOURCE_DIR}/src/include
    ${PROJECT_SOURCE_DIR}/src/data
)

add_subdirectory(tape)
add_subdirectory(processer)