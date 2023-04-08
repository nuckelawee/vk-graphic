#include "global_settings.hpp"

GlobalSettings& GlobalSettings::GetInstance() noexcept {
    static GlobalSettings instance;
    return instance;
}

AppState GlobalSettings::ApplicationState() const noexcept {
    return state_;
}

void GlobalSettings::ChangeApplicationState(AppState state) noexcept {
    state_ = state;
}

GraphicApi GlobalSettings::Api() const noexcept {
    return api_;
}

void GlobalSettings::ChangeApi(GraphicApi api) noexcept {
    api_ = api;
}

uint32_t GlobalSettings::ScreenWidth() const noexcept {
    return screenWidth_;
}

uint32_t GlobalSettings::ScreenHeight() const noexcept {
    return screenHeight_;
}

void GlobalSettings::ScreenResize(uint32_t width, uint32_t height) noexcept {
    screenWidth_ = width;
    screenHeight_ = height;
}
