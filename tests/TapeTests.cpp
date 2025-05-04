#include <gtest/gtest.h>
#include "Tape.h"
#include <filesystem>

class TapeTest : public ::testing::Test {
protected:
    void SetUp() override {
        filename = TEST_INPUT_DIR "test_tape.txt";
    }

    void TearDown() override {
        std::filesystem::remove(filename);
    }

    std::string filename;
    Latencies latencies{{"read", 0}, {"write", 0}, {"shift", 0}, {"rewind", 0}};
};

TEST_F(TapeTest, WriteAndRead) {
    {
        Tape tape(filename, latencies);
        tape << 10 << 20 << 30;
    }

    Tape tape(filename, latencies);
    int val;
    ASSERT_TRUE(tape >> val);
    EXPECT_EQ(val, 10);
    ASSERT_TRUE(tape >> val);
    EXPECT_EQ(val, 20);
    ASSERT_TRUE(tape >> val);
    EXPECT_EQ(val, 30);
    ASSERT_FALSE(tape >> val);
}