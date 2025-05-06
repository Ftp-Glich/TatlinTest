# TatlinTest
test task for Yadro impulse

# Build
 
## download [vcpkg](https://github.com/microsoft/vcpkg)
## cmake --build ${workspaceFolder}/build -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg.cmake --config Release

# Run
## tatlin_test - programm executable
## tests - tests executable
## src/data/latency.conf - latencies configuration (time in milliseconds)
## src/data/input - a place for inputs (if file not found program will generate random input file).
## format:\n read 0\n write 0\n rewind 0\n shift 0

my idea of solution described in algorithm.md in Russian