#include "app_setting.hpp"

//AppSetting* AppSetting::pSetting_;

void AppSetting::Update() {
    statistic_.Update(true);
}

const std::string& AppSetting::Application() const { return appName_; }

void AppSetting::ChangeState(AppState newState) { state_.store(newState); }
void AppSetting::ChangeContext(ContextType newType) { contextType_.store(newType); }
void AppSetting::ChangeWidth(unsigned int newWidth) { windowWidth_.store(newWidth); }
void AppSetting::ChangeHeight(unsigned int newHeight) { windowHeight_.store(newHeight); }

AppState AppSetting::State() const { return state_.load(); }
ContextType AppSetting::Context() const { return contextType_.load(); }
unsigned int AppSetting::Width() const { return windowWidth_.load(); }
unsigned int AppSetting::Height() const { return windowHeight_.load(); }

const Time& AppSetting::Statistic() const { return statistic_; }
Time& AppSetting::Statistic() { return statistic_; }

const vk::Setting& AppSetting::Vulkan() const { return vulkan_setting_; }
vk::Setting& AppSetting::Vulkan() { return vulkan_setting_; }
