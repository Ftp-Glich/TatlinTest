add_library(processer STATIC Processer.cpp)

target_include_directories(processer PUBLIC 
    ${PROJECT_SOURCE_DIR}/src/include
)