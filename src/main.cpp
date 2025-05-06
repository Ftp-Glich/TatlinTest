#include "Processer.h"

int main(int argc, char** argv) {
    if(argc != 3) {
        std::cerr << "wrong number of args" << std::endl;
        return -1;
    }
    Processer processer(100, 100000, "src/data/", argv[1], argv[2], "latency.conf");
    processer.sort();
}