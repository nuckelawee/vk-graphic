#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

class Statistic {

    float curTime_; 
    float deltaTime_ = 0;
    unsigned int framesInSecond_ = 0;
    
private:
    void PrintStatistic();
public:
    void Update(bool isPrint = false);

    float DeltaTime() { return deltaTime_; }
    float curTime() { return curTime_; }
    unsigned int Frames() { return framesInSecond_; }

    Statistic() {}
    ~Statistic() {} 
};
