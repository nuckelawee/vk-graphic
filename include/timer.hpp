#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class App;

class Timer {
    friend class App;

    float currentTime_; 
    float deltaTime_ = 0;
    unsigned int fps_ = 0;
    
private:

public:
    void Update() noexcept;
    static Timer& GetInstance() noexcept;

    float DeltaTime() const noexcept;
    float CurrentTime() const noexcept;
    unsigned int Fps() const noexcept;

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
private:

    Timer() = default;

};
