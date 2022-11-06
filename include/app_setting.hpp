#pragma once

#include <string>

class AppSetting {
    const std::string appName = "Vulkan";
    const std::string confFilepath = "../config/setting.conf";

    unsigned int windowWidth = 800;
    unsigned int windowHeight = 800;

public:

    void ChangeWidth(unsigned int newWidth) { windowWidth = newWidth; }
    void ChangeHeight(unsigned int newHeight) { windowHeight = newHeight; }

    std::string AppName() const { return appName; }
    unsigned int Width() const { return windowWidth; }
    unsigned int Height() const { return windowHeight; }

    AppSetting() {}
    ~AppSetting() {}
};
