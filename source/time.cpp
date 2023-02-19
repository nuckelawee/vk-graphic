#include "time.hpp"

void Time::Update(bool isPrint) {
    static float oneSecond = 0;
    static unsigned int frames = 0;
    float prevTime = 0;
    prevTime = curTime_;
    curTime_ = glfwGetTime();
    deltaTime_ = curTime_ - prevTime;    
    oneSecond += deltaTime_;
    frames++;
    if(oneSecond > 1.0) {
        if(isPrint)
        { PrintInfo(); }
        fps_ = frames;
        oneSecond = 0;
        frames = 0;
    } 
}

void Time::PrintInfo() const {
    std::cout << "FPS (" << fps_ << ") Time ("
        << static_cast<unsigned int>(curTime_) << ")\n";
}
