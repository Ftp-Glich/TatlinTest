#include "TapePool.h"


class Processer {
public:
    explicit Processer(int memory, int number, const std::string& path, const std::string& input, const std::string& output, const std::string& latencies);

    void sort();

    void createSubseq();

    void generateRandomInputFile(const std::string& filename, size_t N, int min_val = -1000000, int max_val = 1000000);

    void mergeSubseq();

    void checkSortition();

    void sortAndWrite(std::vector<int>& vec, int count);

    void writeToTape(const std::vector<int>& vec, const std::string& file);

private:
    int M, N;

    Latencies latencies_m;

    std::string input_name, output_name;

    std::string input_dir, output_dir, tmp_dir;

    void parseLatency(const std::string& file);

    void prepareTempDirectory(const std::string& temp_dir);

    
};