#include "Processer.h"

Processer::Processer(int memory, int number, const std::string& input, const std::string& output, const std::string& latencies)
:M(memory), N(number), input_name(input), output_name(output){
    parseLatency(latencies);
    prepareTempDirectory(tmp_dir);
}


void Processer::prepareTempDirectory(const std::string& temp_dir) {
    namespace fs = std::filesystem;
    try {
        fs::path dir_path(temp_dir);

        if (fs::exists(dir_path)) {
            std::cout << "Clearing temp directory: " << dir_path << "\n";
            for (const auto& entry : fs::directory_iterator(dir_path)) {
                fs::remove_all(entry.path());
            }
        }
        fs::create_directories(dir_path);
        
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "Unknown error preparing temp directory\n";
    }
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

void Processer::writeToTape(const std::vector<int>& vec, const std::string& file) {
    Tape tape(file, latencies_m);
    for(auto& el: vec) tape << el;
}

void Processer::sortAndWrite(std::vector<int>& vec, int count) {
    std::stable_sort(vec.begin(), vec.end());
    std::string filename = tmp_dir + std::to_string(count) + ".txt";
    writeToTape(vec, filename);
}

void Processer::createSubseq() {
    Tape input_tape(input_dir + input_name, latencies_m);
    std::vector<int> tmp(M/4);
    size_t j = 0, count = 0;
    for(size_t i = 0; i < N; ++i) {
        int num;
        input_tape >> num;
        tmp[j] = num;
        ++j;
        if(j == M/4) {
            sortAndWrite(tmp, count);

            ++count;
            j = 0;
        }
    }
    if (j > 0) {
        tmp.resize(j);
        sortAndWrite(tmp, count);
        ++count;
    }
}

size_t findMin(const std::vector<int>& vec, const std::vector<bool>& active) {
    size_t res = 0;
    int min_val = INT32_MAX;
    for(size_t i = 0; i < vec.size(); ++i) {
        if(active[i] && vec[i] < min_val) {
            res = i;
            min_val = vec[i];
        }
    }
    return res;
}

void Processer::mergeSubseq() {
    Tape output_tape(output_dir + output_name, latencies_m);
    int chunk_size = M / 4;
    int tmp_num = (N + chunk_size - 1) / chunk_size;

    std::vector<Tape> tapes;
    tapes.reserve(tmp_num);
    for(int i = 0; i < tmp_num; ++i) {
        tapes.emplace_back(tmp_dir + std::to_string(i) + ".txt", latencies_m);
    }

    std::vector<int> nums(tmp_num);
    std::vector<bool> active(tmp_num, false);

    for(int i = 0; i < tmp_num; ++i) {
        if (tapes[i] >> nums[i]) {
            active[i] = true;
        } else {
            active[i] = false; 
        }
    }

    for(int i = 0; i < N; ++i) {
        size_t idx = findMin(nums, active);
        output_tape << nums[idx];

        if (!(tapes[idx] >> nums[idx])) {
            active[idx] = false;
        }
    }
}


void Processer::sort() {
    createSubseq();
    mergeSubseq();
}