#pragma once

#include <string>
#include <atomic>

#include "vk/vk_setting.hpp"
#include "time.hpp"

enum AppState { APP_STATE_WORK, APP_STATE_STOP, APP_STATE_TERMINATE };
enum ContextType { CONTEXT_TYPE_CAMERA, CONTEXT_TYPE_UNKNOWN };

class AppSetting {

    const std::string confFilepath_ = "../config/setting.conf";
    const std::string appName_ = "Vulkan";

    std::atomic<AppState> state_ { APP_STATE_WORK };
    std::atomic<ContextType> contextType_ { CONTEXT_TYPE_CAMERA };
    std::atomic<uint16_t> windowWidth_ {800};
    std::atomic<uint16_t> windowHeight_ {800};
    vk::Setting vulkan_setting_;
    Time statistic_;

public:
    void Update();

    const std::string& Application() const;

    void ChangeState(AppState newState);
    void ChangeContext(ContextType newType);
    void ChangeWidth(unsigned int newWidth);
    void ChangeHeight(unsigned int newHeight);

    AppState State() const;
    ContextType Context() const;
    unsigned int Width() const;
    unsigned int Height() const;

    const Time& Statistic() const;
    Time& Statistic();
    
    const vk::Setting& Vulkan() const;
    vk::Setting& Vulkan();

    AppSetting() {}
    ~AppSetting() {}

private:
    AppSetting(const AppSetting& setting) = delete;
    AppSetting operator=(const AppSetting& setting) = delete;

};
