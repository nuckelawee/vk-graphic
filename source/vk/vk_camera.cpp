#include "vk/vk_camera.hpp"
#include <cstring>
#include <iostream>

namespace vk {

Camera::Camera(VkDevice device
    , std::array<Buffer, vk::Settings::frames> ubos
    , const glm::vec3& position
    , const glm::vec3& speed
    , const glm::vec3& direction
    , float pitch
    , float yaw
    , float speedFactor
    , float slowFactor)
        : BaseCamera(position, speed, direction, pitch, yaw, speedFactor, slowFactor)
        , ubos_(ubos), device_(device) {

/*
    for(size_t i = 0; i < vk::Settings::frames; ++i) {
        vkMapMemory(device_, ubos_[i].memory
            , 0, sizeof(MvpMatrix), 0, &mappedBuffers_[i]);
    }
*/
}

void Camera::Update() noexcept {
    if(needUpdate_) {
        UpdateMatrices();

        uint32_t currentFrame = vk::Settings::GetInstance().CurrentFrame();
        vkMapMemory(device_, ubos_[currentFrame].memory
            , 0, sizeof(MvpMatrix), 0, &mappedBuffers_[currentFrame]);
        memcpy(mappedBuffers_[currentFrame], &camera_, sizeof(MvpMatrix));
        vkUnmapMemory(device_, ubos_[currentFrame].memory);
        needUpdate_ = false;
    }
}

Camera::~Camera() {
    vkDeviceWaitIdle(device_);
    for(size_t i = 0; i < vk::Settings::frames; ++i) {
        ubos_[i].Destroy(device_);
    }
}

} //vk
