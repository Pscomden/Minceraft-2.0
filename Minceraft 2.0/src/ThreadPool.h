#pragma once

#include "Include.h"

// https://stackoverflow.com/questions/15752659/thread-pooling-in-c11
class ThreadPool {
public:
    void Start();
    void QueueJob(const std::function<void()>& job);
    bool Busy();
    void Stop();
    void ThreadLoop();
private:
    bool should_terminate = false;           // Tells threads to stop looking for jobs
    std::mutex queue_mutex;                  // Prevents data races to the job queue
    std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination 
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> jobs;
};

extern ThreadPool pool;