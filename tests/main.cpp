#include <gtest/gtest.h>
#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    fs::remove_all("tests/data/input");
    fs::remove_all("tests/data/output");
    fs::remove_all("tests/data/tmp");
    fs::create_directories("tests/data/input");
    fs::create_directories("tests/data/output");
    fs::create_directories("tests/data/tmp");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}