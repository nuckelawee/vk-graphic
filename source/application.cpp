#include "application.hpp"

#include "global_settings.hpp"
#include "vk/vk_engine.hpp"
#include "input/input_controller.hpp"
#include "window.hpp"
#include "base_camera.hpp"
#include "timer.hpp"

void Application::Update() {
    while(GlobalSettings::GetInstance().ApplicationState() == AppState::stop);

    controller_->UpdateAll();
    camera_->Update();
    renderer_->Update();
    Timer::GetInstance().Update();
}

void Application::Run() {
    try {
        Init();
        while(!window_->Update()) {
            Update(); 
            glfwPollEvents();
        }
    } catch(std::exception& info) {
        std::cerr << info.what() << '\n';
    }
}

void Application::Init() {
    assert(glfwInit() != 0);
    assert(GlobalSettings::GetInstance().Api() == GraphicApi::vulkan);

    renderer_ = new vk::Engine; 
    window_ = renderer_->Init();
    camera_ = renderer_->CreateCamera();
    controller_ = new input::Controller(*camera_, *window_);
}

Application::~Application() {
    delete camera_;
    delete renderer_;
    delete controller_;
}
