#include "blocking_queue.hpp"

template class BlockingQueue<Task>;
template class BlockingQueue<int>;

template<class T>
void BlockingQueue<T>::Push(const T& newValue) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(newValue);
    isEmpty.notify_one();
}

template<class T>
void BlockingQueue<T>::TryPop(T& value) {
    std::lock_guard<std::mutex> lock(mutex);
    if(queue.empty()) {
        return;
    }
    value = queue.front();
    queue.pop();
}

template<class T>
void BlockingQueue<T>::WaitAndPop(T& value, bool& wait) {
    std::unique_lock<std::mutex> lock(mutex);
    isEmpty.wait(lock, [&](){ return (!queue.empty() || !wait); });

    if(wait == false) { return; }
    value = queue.front();
    queue.pop();
} 

template<class T>
void BlockingQueue<T>::NotifyAll() {
    isEmpty.notify_all();
}

template<class T>
bool BlockingQueue<T>::Empty() {
    return queue.empty();
}
