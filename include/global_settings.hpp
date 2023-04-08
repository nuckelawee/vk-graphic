#pragma once

#include <cstdint>

enum class AppState { work, stop, terminate };
enum class GraphicApi { vulkan };

class GlobalSettings { 

    static constexpr const char* confpath_ = "../configs/setting.conf";
    
    AppState state_ = AppState::work;
    GraphicApi api_ = GraphicApi::vulkan;

    uint32_t screenWidth_ = 800;
    uint32_t screenHeight_ = 800;

public:
    static constexpr const char* applicationName = "Graphic application";

public:

    static GlobalSettings& GetInstance() noexcept;

    AppState ApplicationState() const noexcept;

    GraphicApi Api() const noexcept;

    uint32_t ScreenWidth() const noexcept;
    uint32_t ScreenHeight() const noexcept;

    GlobalSettings(const GlobalSettings&) = delete;
    GlobalSettings& operator=(const GlobalSettings&) = delete;

    void ScreenResize(uint32_t width, uint32_t height) noexcept;
    void ChangeApplicationState(AppState state) noexcept;
    void ChangeApi(GraphicApi api) noexcept;

private:

    GlobalSettings() = default;

};
