add_library(tape_pool STATIC
    TapePool.cpp
)

target_include_directories(tape_pool PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../data
)