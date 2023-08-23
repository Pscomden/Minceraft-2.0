#include "ThreadPool.h"

ThreadPool pool = ThreadPool();

void ThreadPool::Start() {
    const uint32_t num_threads = std::thread::hardware_concurrency() - 1; // Max # of threads the system supports
    std::cout << "Using " << num_threads << " threads for the thread pool.\n";
    threads.resize(num_threads);
    for (uint32_t i = 0; i < num_threads; i++) {
        threads.at(i) = std::thread([this] {this->ThreadLoop(); });
        threads.at(i).detach();
    }
}

void ThreadPool::ThreadLoop() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            mutex_condition.wait(lock, [this] {
                return !jobs.empty() || should_terminate;
                });
            if (should_terminate) {
                return;
            }
            job = jobs.front();
            jobs.pop();
        }
        job();
    }
}

void ThreadPool::QueueJob(const std::function<void()>& job) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        jobs.push(job);
        //std::cout << "Number of jobs: " << jobs.size() << "\n";
    }
    mutex_condition.notify_one();
}

bool ThreadPool::Busy() {
    bool pool_busy;
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        pool_busy = jobs.empty();
    }
    return pool_busy;
}

void ThreadPool::Stop() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        should_terminate = true;
    }
    mutex_condition.notify_all();
    for (std::thread& active_thread : threads) {
        active_thread.join();
    }
    threads.clear();
}