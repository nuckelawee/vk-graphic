#pragma once

#include <thread>
#include <vector>
#include <chrono>

#include "task.hpp"
#include "blocking_queue.hpp"

class ThreadPool  {
    std::vector<std::thread> workers_;
    BlockingQueue<Task> tasks_;
    bool complete_;

private:

    void StartWorkerThreads(size_t threadCount);
    void WorkerRoutine();

public:

    void Submit(const Task& task);

    void Wait();
    void Stop();

    ThreadPool(size_t threadCount);
    ~ThreadPool();

};
