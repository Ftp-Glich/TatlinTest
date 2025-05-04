#include "Processer.h"

int main() {
    Processer processer(100, 10000, "input_big.txt", "output_big.txt", "latency.txt");
    processer.sort();
}