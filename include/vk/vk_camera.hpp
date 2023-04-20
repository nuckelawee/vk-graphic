#pragma once

#include <array>

#include "base_camera.hpp"
#include "vk_buffer.hpp"
#include "vk_settings.hpp"

namespace vk {

class Camera : public BaseCamera {

    std::array<Buffer, vk::Settings::frames> ubos_;
    std::array<void*, vk::Settings::frames> mappedBuffers_; 
    VkDevice device_;

public:
    virtual void Update() noexcept override;

    Camera(VkDevice device
        , std::array<Buffer, vk::Settings::frames> ubos
        , const glm::vec3& position = glm::vec3(0.0f, 0.0f, 3.0f)
        , const glm::vec3& speed = glm::vec3(0.0f, 0.0f, 0.0f)
        , const glm::vec3& direction = glm::vec3(0.0f, 0.0f, -1.0f)
        , float pitch_ = 0.0f
        , float yaw_ = -M_PI/2.0f
        , float speedFactor = 1.5f
        , float slowFactor = 1.1f) noexcept;

    Camera& operator=(const Camera&) = delete;
    Camera(const Camera&) = delete;

    Camera(Camera&&) = default;
    Camera& operator=(Camera&&) = default;

    virtual ~Camera();
};

} //vk
