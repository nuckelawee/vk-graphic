#pragma once

#include <memory>
#include <queue>
#include <condition_variable>
#include <mutex>
#include "task.hpp"

template<class T>
class BlockingQueue {
public:

    void Push(const T& newValue);
    void TryPop(T& value);
    void WaitAndPop(T& value, bool& wait = true);

    void NotifyAll();
    bool Empty();

    BlockingQueue() {}
    ~BlockingQueue() {}

    BlockingQueue& operator=(BlockingQueue&) = delete;
    BlockingQueue(const BlockingQueue&) = delete;

private:
    std::queue<T> queue;
    std::condition_variable isEmpty;
    std::mutex mutex;
};
