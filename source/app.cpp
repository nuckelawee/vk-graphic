#include "app.hpp"

bool App::CheckState() {
    if(setting_.State() == APP_STATE_TERMINATE) {
        pWindow_->CloseWindow();
        return false;
    }
    if(setting_.State() == APP_STATE_STOP) {
        return true;
    }
    return false;
}

void App::Update() {
    bool notWork = true;
    while(notWork) {
        notWork = CheckState();
    }
    controller.Update();
    camera.Update(setting_);
    pRenderer_->Update(static_cast<vk::Surface&>(*pWindow_), camera);
    setting_.Update();
}

void App::Run() {
/*
    threadPool_.Submit(Task([&]() {
        Console::Input(appSetting_);
    }));
*/
    controller.SwitchContext(&camera, CONTEXT_TYPE_CAMERA);
    Init();
    while(!pWindow_->ShouldClosed()) {
        Update(); 
        glfwPollEvents();
    }
}

void App::Init() {
    pWindow_ = new GlfwWindow(setting_);
    pWindow_->CreateWindow(controller);
    pRenderer_ = new vk::Engine(setting_);
    pRenderer_->Init(*pWindow_);
}

App::App() : threadPool_(std::thread::hardware_concurrency()-1) {}

App::~App() {
    pRenderer_->Terminate(*pWindow_);
    delete pRenderer_;
    delete pWindow_;
}
