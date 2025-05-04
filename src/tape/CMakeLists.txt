add_library(tape STATIC
    Tape.cpp
)

target_include_directories(tape PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../data
)