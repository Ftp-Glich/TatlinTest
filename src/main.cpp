#include "Processer.h"

int main() {
    Processer processer(16, 17, "input.txt", "output.txt", "latency.txt");
    processer.sort();
}