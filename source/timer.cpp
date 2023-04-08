#include "timer.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

Timer& Timer::GetInstance() noexcept {
    static Timer instance;
    return instance;
}

float Timer::CurrentTime() const noexcept { return currentTime_; }
float Timer::DeltaTime() const noexcept { return deltaTime_; }
unsigned int Timer::Fps() const noexcept { return fps_; }

void Timer::Update() noexcept {
    const float limit = 1.0f;
    static float oneSecond = 0;
    static unsigned int frames = 0;
    float prevTime = 0;
    prevTime = currentTime_;
    currentTime_ = glfwGetTime();
    deltaTime_ = currentTime_ - prevTime;    
    oneSecond += deltaTime_;
    frames++;
    if(oneSecond > limit) {
        fps_ = frames;
        oneSecond = 0;
        frames = 0;
    } 
}
