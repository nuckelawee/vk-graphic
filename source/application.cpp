#include "application.hpp"

#include "global_settings.hpp"
#include "vk/vk_engine.hpp"
#include "input/input_controller.hpp"
#include "window.hpp"
#include "base_camera.hpp"
#include "timer.hpp"
#include "resource_loader.hpp"

void Application::Update(std::vector<Model>& models) {
    while(GlobalSettings::GetInstance().ApplicationState() == AppState::stop);

    controller_->UpdateAll();
    camera_->Update();
    renderer_->Render(models);
    Timer::GetInstance().Update();
}

void Application::Run() {
    try {
        std::vector<Model>&& models = Init();
        while(!window_->Update()) {
            Update(models); 
            glfwPollEvents();
        }
    } catch(std::exception& info) {
        std::cerr << info.what() << '\n';
    }
}

std::vector<Model> Application::Init() {
    assert(glfwInit() != 0);
    assert(GlobalSettings::GetInstance().Api() == GraphicApi::vulkan);

    std::vector<Modelpath> modelPathes = {
        { "resources/teapot.obj", "resources/picture1.tga" }
    };

    ModelSet&& modelSet = ResourceLoader::LoadModel(std::move(modelPathes));

    renderer_ = new vk::Engine; 
    window_ = renderer_->Init();

    std::vector<Model>&& models = renderer_->Bind(modelSet);
    camera_ = renderer_->CreateCamera();
    controller_ = new input::Controller(*camera_, *window_);
    return models;
}

Application::~Application() {
    delete camera_;
    delete renderer_;
    delete controller_;
}
