#pragma once

#include "vk/vk_engine.hpp"

#include "glfw_window.hpp"

#include "error_manager.hpp"
#include "thread_pool.hpp"
#include "console.hpp"
#include "statistic.hpp"

class App {

    vk::Engine *renderer_;
    GlfwWindow window_;

    ThreadPool threadPool_;
    Statistic statistic_;

    //AppSetting appSetting_;

private:
    bool CheckState();
    void Update();
    void Init();

public:

    void Run();

    App();
    ~App();

};
