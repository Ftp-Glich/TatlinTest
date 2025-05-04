#include <gtest/gtest.h>
#include "Processer.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <cctype>

namespace fs = std::filesystem;

class ProcesserTest : public ::testing::Test {
protected:
    void SetUp() override {
        base_dir = "tests/data/";
        test_name = sanitizeFilename(
            ::testing::UnitTest::GetInstance()->current_test_info()->name()
        );
        
        input_file = test_name + "_input.txt";
        output_file = test_name + "_output.txt";
        latencies_file = test_name + "_latency.conf";
        
        std::ofstream(base_dir + latencies_file) << "read 0\nwrite 0\nshift 0\nrewind 0";
    }

    std::string sanitizeFilename(std::string name) {
        for(auto& c : name) {
            if(!std::isalnum(c)) c = '_';
        }
        return name;
    }

    std::string base_dir;
    std::string test_name;
    std::string input_file;
    std::string output_file;
    std::string latencies_file;
};

TEST_F(ProcesserTest, FullSortIntegration) {
    Processer proc(1024, 100, 
                  base_dir, 
                  input_file, 
                  output_file, 
                  latencies_file);
    
    proc.sort();
    
    std::ifstream out(base_dir + "output/" + output_file);
    int prev, current;
    ASSERT_TRUE(out >> prev);
    while(out >> current) {
        ASSERT_LE(prev, current);
        prev = current;
    }
}

TEST_F(ProcesserTest, CustomInputSort) {
    std::ofstream(base_dir + "input/" + input_file) << "3\n1\n2\n5\n4\n";
    
    Processer proc(256, 5, 
                  base_dir, 
                  input_file, 
                  output_file, 
                  latencies_file);
    
    proc.sort();
    
    std::ifstream out(base_dir + "output/" + output_file);
    std::vector<int> results;
    int val;
    while(out >> val) results.push_back(val);
    ASSERT_EQ(results, (std::vector<int>{1,2,3,4,5}));
}

TEST_F(ProcesserTest, LargeDatasetSort) {
    const int num_elements = 10000;
    
    Processer proc(4096, num_elements, 
                  base_dir, 
                  input_file, 
                  output_file, 
                  latencies_file);
    
    proc.sort();
    
    std::ifstream out(base_dir + "output/" + output_file);
    int count = 0;
    int tmp;
    while(out >> tmp) count++;
    ASSERT_EQ(count, num_elements);
}

TEST_F(ProcesserTest, EmptyFileSort) {
    std::ofstream(base_dir + "input/" + input_file).close();
    
    Processer proc(1024, 0,  
                  base_dir, 
                  input_file, 
                  output_file, 
                  latencies_file);
    
    proc.sort();
    
    std::ifstream out(base_dir + "output/" + output_file);
    int tmp;
    ASSERT_FALSE(out >> tmp) << "Output file should be empty";
}

TEST_F(ProcesserTest, AllEqualElements) {
    const int value = 42;
    const int count = 10;
    
    std::ofstream(base_dir + "input/" + input_file);
    for(int i = 0; i < count; ++i) {
        std::ofstream(base_dir + "input/" + input_file, std::ios::app) 
            << value << "\n";
    }
    
    Processer proc(512, count,
                  base_dir, 
                  input_file, 
                  output_file, 
                  latencies_file);
    
    proc.sort();
    
    std::ifstream out(base_dir + "output/" + output_file);
    int val;
    int read_count = 0;
    while(out >> val) {
        ASSERT_EQ(val, value);
        read_count++;
    }
    ASSERT_EQ(read_count, count);
}

TEST_F(ProcesserTest, NegativeNumbersSort) {
    std::ofstream(base_dir + "input/" + input_file) 
        << "-5\n3\n-2\n0\n-10\n8\n";
    
    Processer proc(256, 6,
                  base_dir, 
                  input_file, 
                  output_file, 
                  latencies_file);
    
    proc.sort();
    
    std::ifstream out(base_dir + "output/" + output_file);
    std::vector<int> expected = {-10, -5, -2, 0, 3, 8};
    std::vector<int> results;
    int val;
    while(out >> val) results.push_back(val);
    ASSERT_EQ(results, expected);
}

TEST_F(ProcesserTest, SingleElementSort) {
    std::ofstream(base_dir + "input/" + input_file) << "100\n";
    
    Processer proc(1024, 1,
                  base_dir, 
                  input_file, 
                  output_file, 
                  latencies_file);
    
    proc.sort();
    
    std::ifstream out(base_dir + "output/" + output_file);
    int val;
    ASSERT_TRUE(out >> val);
    ASSERT_EQ(val, 100);
    ASSERT_FALSE(out >> val);
}

TEST_F(ProcesserTest, InsufficientMemoryHandling) {
    const int num_elements = 100;
    const int memory = 128; 
    
    Processer proc(memory, num_elements,
                  base_dir, 
                  input_file, 
                  output_file, 
                  latencies_file);
    
    proc.sort();
    
    std::ifstream out(base_dir + "output/" + output_file);
    int prev, current;
    int count = 0;
    
    ASSERT_TRUE(out >> prev);
    count++;
    while(out >> current) {
        ASSERT_LE(prev, current);
        prev = current;
        count++;
    }
    ASSERT_EQ(count, num_elements);
}
