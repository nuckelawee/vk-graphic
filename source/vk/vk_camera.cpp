#include "vk/vk_camera.hpp"
#include <cstring>

namespace vk {

Camera::Camera(VkDevice device
    , std::array<Buffer, vk::Settings::frames> ubos
    , const glm::vec3& position
    , const glm::vec3& speed
    , const glm::vec3& direction
    , float pitch
    , float yaw
    , float speedFactor
    , float slowFactor) noexcept
        : BaseCamera(position, speed, direction, pitch, yaw, speedFactor, slowFactor)
        , ubos_(ubos), device_(device) {}

void Camera::Update() noexcept {
    UpdateMatrices();

    uint32_t currentFrame = vk::Settings::GetInstance().CurrentFrame();
    vkMapMemory(device_, ubos_[currentFrame].memory
        , 0, sizeof(MvpMatrix), 0, &mappedBuffers_[currentFrame]);
    memcpy(mappedBuffers_[currentFrame], &camera_, sizeof(MvpMatrix));
    vkUnmapMemory(device_, ubos_[currentFrame].memory);
}

Camera::~Camera() {
    vkDeviceWaitIdle(device_);
    for(size_t i = 0; i < vk::Settings::frames; ++i) {
        ubos_[i].Destroy(device_);
    }
}

} //vk
