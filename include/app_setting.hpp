#pragma once

#include <string>
#include <atomic>

enum AppState { APP_WORK, APP_STOP, APP_TERMINATE };

class AppSetting {

    static int frad;
    static AppSetting *pSetting_;
    const std::string confFilepath_ = "../config/setting.conf";

public:
    static constexpr char pAppName[] = "Vulkan";
    static const unsigned int frames = 2;

private:

    std::atomic<AppState> state_ {APP_WORK};
    std::atomic<uint16_t> windowWidth_ {800};
    std::atomic<uint16_t> windowHeight_ {800};

public:

    void ChangeState(AppState newState);
    void ChangeWidth(unsigned int newWidth);
    void ChangeHeight(unsigned int newHeight);

    AppState State();
    unsigned int Width() const;
    unsigned int Height() const;
    
    static void StartUp();
    static void ShutDown();
    static AppSetting& Get();

private:
    AppSetting(const AppSetting&) = delete;
    AppSetting& operator=(const AppSetting&) = delete;
    
    AppSetting() {}
    ~AppSetting() {}
};
