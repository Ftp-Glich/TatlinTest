add_library(tape STATIC tape.cpp)

target_include_directories(tape PRIVATE 
    ${PROJECT_SOURCE_DIR}/src/include
    ${PROJECT_SOURCE_DIR}/src/data
)