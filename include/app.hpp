#pragma once

#include "vk/vk_engine.hpp"

#include "glfw_window.hpp"
#include "camera.hpp"

#include "error_manager.hpp"
#include "thread_pool.hpp"
#include "console.hpp"


class App {
    AppSetting setting_;

    vk::Engine *pRenderer_;
    Camera camera;
    GlfwWindow *pWindow_;

    ThreadPool threadPool_;

private:
    bool CheckState();
    void Update();
    void Init();

public:

    void Run();

    App();
    ~App();

};
