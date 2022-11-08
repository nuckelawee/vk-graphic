#include "thread_pool.hpp"

ThreadPool::ThreadPool(size_t threadCount) {
    StartWorkerThreads(threadCount);
}

void ThreadPool::StartWorkerThreads(size_t threadCount) {
    complete = true;
    for(size_t i = 0; i < threadCount; i++) {
        workers_.emplace_back([this]() {
            WorkerRoutine(); 
        });
    }
}

void ThreadPool::WorkerRoutine() {
    while(complete) {
        Task task;
        tasks_.WaitAndPop(task, complete);
        if(task.Exist()) {
            task();
        }
    }
}

void ThreadPool::Submit(const Task& task) {
    tasks_.Push(task); 
}

void ThreadPool::Wait() {
    while(!tasks_.Empty()) {}
}

void ThreadPool::Stop() {
    complete = false;
    tasks_.NotifyAll();
    for(size_t i = 0; i < workers_.size(); i++) {
        workers_[i].join();
    }
}

ThreadPool::~ThreadPool() {
    Stop();
}
