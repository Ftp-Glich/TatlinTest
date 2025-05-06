#include "TapePool.h"

TapePool::TapePool(const Latencies& lats, const std::string& path, size_t M, size_t group_size, size_t max_open_files):
    group_size_(group_size), max_open_files_(max_open_files),
    latencies_(lats), tmp_dir(path + "tmp/") {
        if (group_size_ > max_open_files_) {
            group_size_ = max_open_files_; 
        }
    
        if (max_open_files_ > M / 4) {
            max_open_files_ = M / 4; 
        }
        workers_.reserve(std::thread::hardware_concurrency());
    }

void TapePool::start(const std::string& output) {
    output_name_ = output;
    running.store(true);
    for(size_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
        workers_.emplace_back(&TapePool::worker_thread, this);
    }
    
}

void TapePool::wait() {
    all_tasks_.store(true);
    if (task_queue_.empty() && ready.size() <= 1 && active_count_ == 0) {
        if(ready.size() == 0) Tape tape(output_name_, latencies_);
        else finalize_merge(std::move(ready[0]));
    }
    else {
        std::unique_lock lock(mtx_);
        cv_done_.wait(lock);
        lock.unlock();
    }
    running.store(false);
    cv_.notify_all();
    cv_files_.notify_all();
    for(auto& th: workers_) th.join();
}

void TapePool::schedule_merge_task(std::vector<std::unique_ptr<Tape>>&& inputs, const std::string& output_name) {
    auto output = acquire_tape(output_name);
    MergeTask task(std::move(inputs), std::move(output));
    std::lock_guard lock(mtx_);
    task_queue_.push(std::move(task));
    cv_.notify_one();
}

std::unique_ptr<Tape> TapePool::acquire_tape(const std::string& filename) {
    std::unique_lock lock(mtx_); 
    
    if (!tape_recycle_bin_.empty()) {
        auto tape = std::move(tape_recycle_bin_.front());
        tape_recycle_bin_.pop();
        tape->reopen(); 
        return tape;
    }
     cv_files_.wait(lock, [&] { 
        return opened_files_.load() < max_open_files_ || !running.load(); });
    
    if (!running.load()) return nullptr;
    opened_files_++;
    return std::make_unique<Tape>(filename, latencies_);
}

void TapePool::release_tape(std::unique_ptr<Tape>&& tape) {
    std::unique_lock lock(mtx_);
    opened_files_--;
    tape->close();
    tape_recycle_bin_.push(std::move(tape));
    cv_files_.notify_all();

}

void TapePool::finalize_merge(std::unique_ptr<Tape>&& tape) {
    tape->close();
    std::string src = tape->get_filename();
    namespace fs = std::filesystem;
    fs::rename(fs::path(src), fs::path(output_name_));
    cv_done_.notify_one();
}

void TapePool::submit(std::unique_ptr<Tape>&& tape) {
    std::unique_lock lock(mtx_ready_);
    tape->setToStart();
    ready.emplace_back(std::move(tape));
    if ((ready.size() == group_size_) || (ready.size() > 1 && ready.size() <= group_size_ && active_count_ == 0 && all_tasks_.load())) {
        
        std::vector<std::unique_ptr<Tape>> new_group;
        new_group.reserve(group_size_);
        
        for (size_t i = 0; i < group_size_ && !ready.empty(); ++i) {
            new_group.push_back(std::move(ready.back()));
            ready.pop_back();
        }
        
        auto output_name = tmp_dir + "merge_" + std::to_string(tmp_id_++) + ".txt";
        schedule_merge_task(std::move(new_group), output_name);
    }
    if (task_queue_.empty() && ready.size() == 1 && active_count_ == 0 && all_tasks_.load()) {
        finalize_merge(std::move(ready[0]));
    }
}

void TapePool::worker_thread() {
    while(running.load()) {
        std::unique_lock lock(mtx_);
        cv_.wait(lock, [&] { 
            return !task_queue_.empty() || !running.load(); 
        });

        if(!running.load() && task_queue_.empty()) {
            cv_done_.notify_all();
            return;
        }
        auto task = std::move(task_queue_.front());
        task_queue_.pop();
        active_count_++;
        lock.unlock();
        perform_kway_merge(task);
        for(auto& tape : task.inputs) {
            release_tape(std::move(tape));
        }
        active_count_--;
        submit(std::move(task.output));
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

void TapePool::perform_kway_merge(const MergeTask& task) {
    size_t size = task.inputs.size();
    opened_files_.fetch_add(size + 1);
    std::vector<int> nums(size);
    std::vector<bool> active(size, false);
    int active_count = 0;
    std::optional<int> prev{std::nullopt};
    for (size_t i = 0; i < size; ++i) {
        if (*(task.inputs[i]) >> nums[i]) {
            active[i] = true;
            active_count++;
        }
    }
    while (active_count > 0) {
        size_t idx = findMin(nums, active);
        if(prev != std::nullopt) {
            if(prev.value() > nums[idx]) {
                assert(prev.value() <= nums[idx]);
            }
        }
        *(task.output) << nums[idx];
        prev = nums[idx];
        if (!(*(task.inputs[idx]) >> nums[idx])) {
            active[idx] = false; 
            active_count--;
        }
    }
}



