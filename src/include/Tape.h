#include <string>
#include <fstream>
#include <sstream> 
#include <thread>
#include <chrono>
#include <iostream>
#include <map>

using Latencies = std::map<std::string, int>;

class Tape {

public:
    explicit Tape(const std::string& file, const Latencies& latency);

    void operator<<(int buffer);

    void operator>>(int& data);

    void rewind(int dist);

    void setToStart();

private:
    Latencies latency_m;
    std::string filename;
    std::fstream stream;
};