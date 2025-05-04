cmake_minimum_required(VERSION 3.20)
project(tests)

find_package(GTest REQUIRED CONFIG)

file(MAKE_DIRECTORY 
    ${CMAKE_CURRENT_SOURCE_DIR}/data/input
    ${CMAKE_CURRENT_SOURCE_DIR}/data/output
    ${CMAKE_CURRENT_SOURCE_DIR}/data/tmp
)

add_executable(tests
    TapeTests.cpp
    ProcesserTests.cpp
    main.cpp
)

target_link_libraries(tests
    PRIVATE
        GTest::gtest 
        GTest::gtest_main
        tatlin_lib
)

target_include_directories(tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../data
)

target_include_directories(tests PRIVATE
    ${PROJECT_SOURCE_DIR}/src/include
)

include(GoogleTest)
gtest_discover_tests(tests)