#include "vk/vk_settings.hpp"
#include "global_settings.hpp"

namespace vk {

Settings& Settings::GetInstance() noexcept {
    static Settings instance;
    return instance;
}

const std::array<VkClearValue, 2>& Settings::ClearValues() const noexcept {
    return clearValues_;
}

std::array<VkClearValue, 2> Settings::ClearValues() noexcept {
    return clearValues_;
}

const VkExtent2D& Settings::Extent() const noexcept {
    return extent_;
}
void Settings::ChangeExtent(VkExtent2D extent) noexcept {
    extent_ = extent;
    GlobalSettings::GetInstance().ScreenResize(extent_.width, extent_.height);
}
 
unsigned int Settings::CurrentFrame() const noexcept {
    return currentFrame_;
}

unsigned int& Settings::CurrentFrame() noexcept {
    return currentFrame_;
}

uint32_t Settings::ImageIndex() const noexcept {
    return imageIndex_;
}
uint32_t& Settings::ImageIndex() noexcept {
    return imageIndex_;
}

} //vk
