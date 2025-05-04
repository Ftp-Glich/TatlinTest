#include "Processer.h"

int main() {
    Processer processer(100, 10000, "src/data/", "input_big.txt", "output_big.txt", "latency.conf");
    processer.sort();
}