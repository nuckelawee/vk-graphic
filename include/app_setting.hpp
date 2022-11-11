#pragma once

#include <string>
#include <atomic>

enum AppState { APP_WORK, APP_STOP, APP_TERMINATE };

class AppSetting {
    std::atomic<AppState> state_ {APP_STOP};

    const std::string appName_ = "Vulkan";
    const std::string confFilepath_ = "../config/setting.conf";

    std::atomic<uint16_t> windowWidth_ {800};
    std::atomic<uint16_t> windowHeight_ {800};

public:

    void ChangeState(AppState newState) { state_.store(newState); }
    void ChangeWidth(unsigned int newWidth) { windowWidth_.store(newWidth); }
    void ChangeHeight(unsigned int newHeight) { windowHeight_.store(newHeight); }

    AppState State() { return state_.load(); }
    std::string AppName() const { return appName_; }
    unsigned int Width() const { return windowWidth_.load(); }
    unsigned int Height() const { return windowHeight_.load(); }
    
    AppSetting() {}
    ~AppSetting() {}
};
