#include "statistic.hpp"

void Statistic::Update(bool isPrint) {
    static float fpsTime = 0;
    static unsigned int frames = 0;
    float prevTime = 0;
    prevTime = curTime_;
    curTime_ = glfwGetTime();
    deltaTime_ = curTime_ - prevTime;    
    fpsTime += deltaTime_;
    //std::cout << "Frame: " << frames << '\n';
    frames++;
    if(fpsTime > 1.0) {
        if(isPrint)
        { PrintStatistic(); }
        framesInSecond_ = frames;
        fpsTime = 0;
        frames = 0;
    } 
}

void Statistic::PrintStatistic() {
    std::cout << "FPS (" << framesInSecond_ << ") Time ("
        << static_cast<unsigned int>(curTime_) << ")\n";
}
