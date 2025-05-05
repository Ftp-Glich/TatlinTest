#include "Tape.h"
#include <queue>
#include <unordered_map>

struct MergeTask {
    std::vector<std::unique_ptr<Tape>> inputs;
    std::unique_ptr<Tape> output;
    std::atomic<bool> done{false};
    MergeTask(std::vector<std::unique_ptr<Tape>>&& in, std::unique_ptr<Tape>&& out)
    : inputs(std::move(in)), output(std::move(out)) {}
    MergeTask(MergeTask&& other) noexcept {
        if (this != &other) {
            inputs = std::move(other.inputs);
            output = std::move(other.output);
            done.store(other.done.load());
        }
    }
    MergeTask(const MergeTask&) = delete;
    MergeTask& operator=(const MergeTask&) = delete;
};


class TapePool {
    public:
        TapePool(const Latencies& lats, const std::string& path, size_t M);
        
        void merge(std::vector<std::unique_ptr<Tape>>&& input_tapes, const std::string& output);
        
    private:
        void worker_thread();
        std::unique_ptr<Tape> acquire_tape(const std::string& filename);
        void schedule_merge_task(std::vector<std::unique_ptr<Tape>>&& inputs, const std::string& output);   
        void try_make_task(std::unique_ptr<Tape>&& tape);
        void release_tape(std::unique_ptr<Tape>&& tape);
        void perform_kway_merge(const MergeTask& task);
        void finalize_merge(std::unique_ptr<Tape>&& tape);

        size_t max_open_files_;
        size_t group_size_;
        Latencies latencies_;
        std::string output_name_;

        std::atomic<uint32_t> active_count_{0};        
        std::atomic<uint32_t> tmp_id_{0};
        std::atomic<uint32_t> opened_files_{0};
        std::atomic<bool> running{false};

        std::queue<std::unique_ptr<Tape>> tape_recycle_bin_; 
        std::queue<MergeTask> task_queue_;
        std::vector<std::unique_ptr<Tape>> ready;

        std::string tmp_dir;

        std::vector<std::thread> workers_;
        std::mutex mtx_;
        std::mutex mtx_ready_;
        std::condition_variable cv_;
        std::condition_variable cv_files_;
        std::condition_variable cv_done_;

    };