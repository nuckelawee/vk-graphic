#include "app_setting.hpp"

//AppSetting* AppSetting::pSetting_;

void AppSetting::Update() {
    statistic.Update(true);
}

AppState AppSetting::State() { return state_.load(); }
unsigned int AppSetting::Width() const { return windowWidth_.load(); }
unsigned int AppSetting::Height() const { return windowHeight_.load(); }

void AppSetting::ChangeState(AppState newState) { state_.store(newState); }
void AppSetting::ChangeWidth(unsigned int newWidth) { windowWidth_.store(newWidth); }
void AppSetting::ChangeHeight(unsigned int newHeight) { windowHeight_.store(newHeight); }

