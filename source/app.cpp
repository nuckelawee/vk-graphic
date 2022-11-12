#include "app.hpp"

bool App::CheckState() {
    if(globalSetting_.State() == APP_TERMINATE) {
        window_.CloseWindow();
        return false;
    }
    if(globalSetting_.State() == APP_STOP) {
        return true;
    }
    return false;
}

void App::Update() {
    bool notWork = true;
    while(notWork) {
        notWork = CheckState();
    }
    engine_.Update(globalSetting_, window_);
    statistic_.Update();
}

void App::Run() {
/*
    threadPool_.Submit(Task([&]() {
        Console::Input(globalSetting_);
    }));
*/
    Init();
    while(!window_.ShouldClosed()) {
        Update(); 
        glfwPollEvents();
    }
    globalSetting_.ChangeState(APP_TERMINATE);
}

void App::Init() {
    window_.CreateWindow(globalSetting_);
    engine_.Init(globalSetting_, window_);
}

App::App() : threadPool_(std::thread::hardware_concurrency()-1) {}

App::~App() {
    engine_.Terminate(window_);
}
