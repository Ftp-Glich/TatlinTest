#include "Tape.h"

class Processer {
public:
    explicit Processer(int memory, int number, const std::string& input, const std::string& output, const std::string& latencies);

    void sort();

private:
    int M, N;

    Latencies latencies_m;

    std::string input_name, output_name;

    std::string input_dir{"src/data/input/"}, 
                output_dir{"src/data/output/"}, 
                tmp_dir{"src/data/tmp/"};

    void parseLatency(const std::string& file);

    void sortAndWrite(std::vector<int>& vec, int count);

    void writeToTape(const std::vector<int>& vec, const std::string& file);

    void prepareTempDirectory(const std::string& temp_dir);

    void createSubseq();

    void mergeSubseq();
};