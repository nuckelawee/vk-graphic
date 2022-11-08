#pragma once

#include <string>

enum AppState { APP_WORK, APP_STOP, APP_TERMINATE };

class AppSetting {
    AppState state_ = APP_STOP;

    const std::string appName_ = "Vulkan";
    const std::string confFilepath_ = "../config/setting.conf";

    unsigned int windowWidth_ = 800;
    unsigned int windowHeight_ = 800;

public:

    void ChangeState(AppState newState) { state_ = newState; }
    void ChangeWidth(unsigned int newWidth) { windowWidth_ = newWidth; }
    void ChangeHeight(unsigned int newHeight) { windowHeight_ = newHeight; }

    AppState State() { return state_; }
    std::string AppName() const { return appName_; }
    unsigned int Width() const { return windowWidth_; }
    unsigned int Height() const { return windowHeight_; }
    

    AppSetting() {}
    ~AppSetting() {}
};
