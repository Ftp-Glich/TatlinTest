#include <string>
#include <fstream>
#include <sstream> 
#include <thread>
#include <chrono>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <cassert>
#include <random>
#include <stdexcept>
#include <map>

using Latencies = std::map<std::string, int>;

class Tape {

public:
    explicit Tape(const std::string& file, const Latencies& latency);

    Tape(const Tape&) = delete;
    Tape& operator=(const Tape&) = delete;
    
    Tape(Tape&& other) noexcept;
    Tape& operator=(Tape&& other) noexcept;

    ~Tape();

    void operator<<(int buffer);

    bool operator>>(int& data);

    void rewind(int dist);

    void setToStart();

    void close();
    
    void open();

private:
    Latencies latency_m;
    std::string filename;
    std::fstream stream;
};