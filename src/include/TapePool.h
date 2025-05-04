#include "Tape.h"
#include <queue>
#include <unordered_map>

struct MergeTask {
    std::vector<Tape*> inputs;
    Tape* output;
    std::atomic<bool> done{false};
};

class TapePool {
    public:
        TapePool(size_t max_open_files, size_t buffer_size, const Latencies& lats);
        ~TapePool();
        
        void merge(std::vector<std::string>& input_tapes, const std::string& output);
        
    private:
        void worker_thread();
        Tape* acquire_tape(const std::string& filename);
        void release_tape(Tape* tape);
        void schedule_merge_task(const std::vector<std::string>& inputs, const std::string& output);
        
        size_t max_open_files_;
        size_t buffer_size_;
        Latencies latencies_;
        
        std::queue<std::string> tape_recycle_bin_; 
        std::unordered_map<std::string, std::unique_ptr<Tape>> open_tapes_;
        std::queue<MergeTask> task_queue_;
        
        std::vector<std::thread> workers_;
        std::mutex mtx_;
        std::condition_variable cv_;
        bool stop_flag_ = false;
    };