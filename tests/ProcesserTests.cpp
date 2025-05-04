#include <gtest/gtest.h>
#include "Processer.h"

class ProcesserTest : public ::testing::Test {
protected:
    void SetUp() override {
        input_file = TEST_INPUT_DIR "input.txt";
        output_file = TEST_OUTPUT_DIR "output.txt";
        tmp_dir = TEST_TMP_DIR;
    }

    std::string input_file;
    std::string output_file;
    std::string tmp_dir;
    const std::string latencies_config = "latencies.conf";
};

TEST_F(ProcesserTest, FullSortTest) {
    std::ofstream(TEST_INPUT_DIR + latencies_config) << "read 0\nwrite 0\nshift 0\nrewind 0";
    
    Processer proc(/*memory=*/ 1024, 
                  /*number=*/ 100, 
                  input_file, 
                  output_file, 
                  TEST_INPUT_DIR + latencies_config);
    
    proc.sort();
    
    std::ifstream out(output_file);
    int prev, current;
    out >> prev;
    while(out >> current) {
        ASSERT_LE(prev, current);
        prev = current;
    }
}