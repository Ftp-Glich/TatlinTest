#include "Tape.h"

Tape::Tape(const std::string& file, const Latencies& latency)
    : filename(file), stream(filename, std::ios::in | std::ios::out | std::ios::app) {  
    if (!stream.is_open()) { 
        stream.open(filename, std::ios::out);  
        stream.close();
        stream.open(filename, std::ios::in | std::ios::out); 
        if (!stream.is_open()) {
            std::cerr << "Error: Cannot create file " << filename << std::endl;
        }
    }
}

Tape::Tape(Tape&& other) noexcept 
    : filename(std::move(other.filename)),
      latency_m(std::move(other.latency_m)),
      stream(std::move(other.stream)) {
}

Tape& Tape::operator=(Tape&& other) noexcept {
    if (this != &other) {
        filename = std::move(other.filename);
        latency_m = std::move(other.latency_m);
        stream = std::move(other.stream);
    }
    return *this;
}

void Tape::close() { stream.close(); }

void Tape::open() { stream.open(filename, std::ios::in | std::ios::out); }

void Tape::operator<<(int buffer) {
    auto start = std::chrono::high_resolution_clock::now();
    stream << buffer << "\n";
    auto diff = std::chrono::milliseconds(latency_m["write"] + latency_m["shift"]) - 
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
    if(diff.count() > 0) std::this_thread::sleep_for(diff);
}

bool Tape::operator>>(int& data) {
    auto start = std::chrono::high_resolution_clock::now();
    if (stream >> data) {
        auto diff = std::chrono::milliseconds(latency_m["read"] + latency_m["shift"]) - 
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
        if (diff.count() > 0) std::this_thread::sleep_for(diff);
        return true;
    } else {
        return false;
    }
}

void Tape::rewind(int dist) {
    auto start = std::chrono::high_resolution_clock::now();
    int pos = stream.tellp();
    stream.seekp(pos + dist);
    auto diff = std::chrono::milliseconds(latency_m["rewind"]) - 
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
    if(diff.count() > 0) std::this_thread::sleep_for(diff);
}

void Tape::setToStart() {
    this->rewind(-stream.tellp());
}

Tape::~Tape() {
    if(stream.is_open()) stream.close();
}