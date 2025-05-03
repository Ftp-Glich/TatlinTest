find_package(GTest REQUIRED)

add_executable(tests 
    main.cpp
)

target_link_libraries(tests 
    PRIVATE 
        GTest::GTest 
        GTest::Main
)

include(GoogleTest)
gtest_discover_tests(tests)