#pragma once

#include <string>
#include <atomic>

#include "statistic.hpp"
#include "vk/vk_setting.hpp"

enum AppState { APP_WORK, APP_STOP, APP_TERMINATE };

class AppSetting {

    static int frad;
    const std::string confFilepath_ = "../config/setting.conf";
    const std::string appName = "Vulkan";

    std::atomic<AppState> state_ {APP_WORK};
    std::atomic<uint16_t> windowWidth_ {800};
    std::atomic<uint16_t> windowHeight_ {800};
    vk::Setting vulkan_setting;
    Statistic statistic;

public:
    void Update();

    const std::string& Application() const { return appName; }

    void ChangeState(AppState newState);
    void ChangeWidth(unsigned int newWidth);
    void ChangeHeight(unsigned int newHeight);

    AppState State();
    unsigned int Width() const;
    unsigned int Height() const;
    
    vk::Setting& Vulkan() { return vulkan_setting; }
    vk::Setting Vulkan() const { return vulkan_setting; }

    //AppSetting(const AppSetting& setting) {}
    AppSetting() {}
    ~AppSetting() {}
};
