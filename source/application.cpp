#include "application.hpp"

#include "global_settings.hpp"
#include "vk/vk_engine.hpp"
#include "input/input_controller.hpp"
#include "window.hpp"
#include "base_camera.hpp"
#include "timer.hpp"

void Application::Update() {
    while(GlobalSettings::GetInstance().ApplicationState() == AppState::stop);

    controller_->Update();
    camera_->Update();
    renderer_->Update();
    Timer::GetInstance().Update();
}

void Application::Run() {
    Init();
    while(!window_->Update()) {
        Update(); 
        glfwPollEvents();
    }
}

void Application::Init() {
    if(glfwInit() == 0) {
        std::cerr << "Failed to init glfw library\n";
        std::terminate(); 
    }
    if(GlobalSettings::GetInstance().Api() == GraphicApi::vulkan) {
        renderer_ = new vk::Engine; 
        window_ = renderer_->Init();
        camera_ = renderer_->CreateCamera();
    } else {
        std::terminate();  
    }
    controller_ = new input::Controller();
    window_->SetWindowUserPointer(controller_);
    glfwPollEvents();
    controller_->Update();
    controller_->SwitchContext(input::ContextType::camera, camera_);
}

Application::~Application() {
    delete camera_;
    delete renderer_;
    delete controller_;
}
