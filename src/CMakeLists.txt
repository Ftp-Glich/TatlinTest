add_subdirectory(tape)
add_subdirectory(processer)
add_subdirectory(tape_pool)

add_library(tatlin_lib INTERFACE)

target_link_libraries(tatlin_lib INTERFACE
    tape
    processer
    tape_pool
)

add_executable(tatlin_test main.cpp)

target_link_libraries(tatlin_test PRIVATE 
    tatlin_lib
)