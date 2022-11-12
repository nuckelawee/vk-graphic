#pragma once

#include "vk/vk_engine.hpp"
#include "thread_pool.hpp"
#include "console.hpp"
#include "statistic.hpp"

class App {

    vk::VulkanEngine engine_;
    Window window_;
    AppSetting globalSetting_;
    ThreadPool threadPool_;
    Statistic statistic_;

private:
    bool CheckState();
    void Update();
    void Init();

public:

    void Run();

    App();
    ~App();

};
