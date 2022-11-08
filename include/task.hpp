#pragma once

#include <functional>
class Task {
public:

    std::function<void()>& Access()
    { return func_; }

    void operator()()
    { func_(); }
    
    bool Exist() {
        if(func_) { return true; }
        return false;   
    }

    Task(const std::function<void()>& func) : func_(func) {}
    Task() : func_() {}
    ~Task() {}

private:
    std::function<void()> func_;
};
