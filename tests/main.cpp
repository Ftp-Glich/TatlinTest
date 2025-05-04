#include <gtest/gtest.h>
#include <cstdlib>

int main(int argc, char** argv) {
    std::system("del /Q tests\\data\\input\\*");
    std::system("del /Q tests\\data\\output\\*");
    std::system("del /Q tests\\data\\tmp\\*");
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}