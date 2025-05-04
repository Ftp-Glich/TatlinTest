#include <gtest/gtest.h>
#include "Tape.h"
#include <filesystem>

class TapeTest : public ::testing::Test {
protected:
    void SetUp() override {
        filename =  "tests/data/input/test_tape.txt";
    }

    void TearDown() override {
        std::filesystem::remove(filename);
    }

    std::string filename;
    Latencies latencies{{"read", 0}, {"write", 0}, {"shift", 0}, {"rewind", 0}};
};

TEST_F(TapeTest, RewindToStart) {
    Tape tape(filename, latencies);
    tape << 100 << 200;
    tape.setToStart();
    
    int val;
    ASSERT_TRUE(tape >> val);
    EXPECT_EQ(val, 100);
}

TEST_F(TapeTest, RewindOperation) {
    Tape tape(filename, latencies);
    tape << 10 << 20 << 30;
    tape.rewind(-2 * sizeof(int)); 
    
    int val;
    ASSERT_TRUE(tape >> val);
    EXPECT_EQ(val, 20);
}

TEST_F(TapeTest, EmptyFileRead) {
    Tape tape(filename, latencies);
    int val;
    ASSERT_FALSE(tape >> val);
}

TEST_F(TapeTest, SequentialOperations) {
    Tape tape(filename, latencies);
    tape << 5 << 15 << 25;
    tape.setToStart();
    
    std::vector<int> results;
    int val;
    while(tape >> val) {
        results.push_back(val);
    }
    
    ASSERT_EQ(results, (std::vector<int>{5, 15, 25}));
}

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