#include <gtest/gtest.h>
#include "Tape.h"
#include <filesystem>
#include <chrono>

class TapeTest : public ::testing::Test {
protected:
    using Clock = std::chrono::high_resolution_clock;
    void SetUp() override {
        filename =  "tests/data/input/test_tape.txt";
    }

    void TearDown() override {
        std::filesystem::remove(filename);
    }

    std::string filename;
    Latencies latencies{{"read", 0}, {"write", 0}, {"shift", 0}, {"rewind", 0}};

    void assertDelay(long long operationTime, int expectedDelay, double tolerance = 0.2) {
        const long long lower = expectedDelay * (1 - tolerance);
        const long long upper = expectedDelay * (1 + tolerance);
        ASSERT_GE(operationTime, lower) << "Operation faster than expected";
        ASSERT_LE(operationTime, upper) << "Operation slower than expected";
    }
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

TEST_F(TapeTest, WriteOperationDelay) {
    const int WRITE_DELAY = 50;
    const int SHIFT_DELAY = 20;
    latencies = {{"write", WRITE_DELAY}, {"shift", SHIFT_DELAY}, {"read", 0}, {"rewind", 0}};

    Tape tape(filename, latencies);
    auto start = Clock::now();
    tape << 42;
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count();
    
    assertDelay(duration, WRITE_DELAY + SHIFT_DELAY);
}

TEST_F(TapeTest, ReadOperationDelay) {
    const int READ_DELAY = 40;
    const int SHIFT_DELAY = 15;
    latencies = {{"read", READ_DELAY}, {"shift", SHIFT_DELAY}, {"write", 0}, {"rewind", 0}};

    {
        Tape tape(filename, latencies);
        tape << 123;
    }

    Tape tape(filename, latencies);
    int val;
    auto start = Clock::now();
    tape >> val;
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count();
    
    assertDelay(duration, READ_DELAY + SHIFT_DELAY);
}

TEST_F(TapeTest, RewindOperationDelay) {
    const int REWIND_DELAY = 100;
    latencies = {{"rewind", REWIND_DELAY}, {"read", 0}, {"write", 0}, {"shift", 0}};

    Tape tape(filename, latencies);
    auto start = Clock::now();
    tape.rewind(10); 
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count();
    
    assertDelay(duration, REWIND_DELAY);
}

TEST_F(TapeTest, ZeroLatencyOperations) {
    latencies = {{"read", 0}, {"write", 0}, {"shift", 0}, {"rewind", 0}};

    Tape tape(filename, latencies);
    auto start = Clock::now();
    tape << 100;
    tape.setToStart();
    int val;
    tape >> val;
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count();
    
    ASSERT_LE(duration, 10) << "Operations with zero latency took too long";
}