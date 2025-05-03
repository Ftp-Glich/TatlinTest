#include "Processer.h"

Processer::Processer(int memory, int number, const std::string& input, const std::string& output, const std::string& latencies)
:M(memory), N(number), input_name(input), output_name(output){
    parseLatency(latencies);
}

void Processer::parseLatency(const std::string& file) {
    std::ifstream config(file);
    std::string buffer;
    while(std::getline(config, buffer)) {
        std::stringstream ss(buffer);
        std::string name;
        int val;
        ss >> name >> val;
        latencies_m[name] = val;
    }
}

void Processer::sort() {
    Tape input_tape(input_dir + input_name, latencies_m);
    for(size_t i = 0; i < N; ++i) {
        int number;
        input_tape >> number;
        std::cout << number << std::endl;
    }
}