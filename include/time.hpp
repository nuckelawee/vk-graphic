#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

class Time {

    float curTime_; 
    float deltaTime_ = 0;
    unsigned int fps_ = 0;
    
private:

    void PrintInfo() const;

public:
    void Update(bool isPrint = false);

    float DeltaTime() const { return deltaTime_; }
    float CurTime() const { return curTime_; }
    unsigned int Fps() const { return fps_; }

    Time() {}
    ~Time() {} 
};
