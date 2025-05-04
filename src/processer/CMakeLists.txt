add_library(processer STATIC
    Processer.cpp
)

target_include_directories(processer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../data
)