#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>

#include "mvp_matrix.hpp"

class BaseCamera {
protected:

    MvpMatrix camera_{};
    glm::vec3 position_;
    glm::vec3 speed_; 
    glm::vec3 direction_;
    glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);

    float pitch_;
    float yaw_;

    float speedFactor_;
    float slowFactor_;

protected:

    void UpdateMatrices() noexcept;

public:

    virtual void Update() noexcept;
    const MvpMatrix* Matrix() const noexcept;

    void Rotate(float deltaPitch, float deltaYaw) noexcept;

    void MoveForward() noexcept;
    void MoveBack() noexcept;
    void MoveRight() noexcept;
    void MoveLeft() noexcept;
    void MoveUp() noexcept;
    void MoveDown() noexcept;

    BaseCamera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 3.0f)
        , const glm::vec3& speed = glm::vec3(0.0f, 0.0f, 0.0f)
        , const glm::vec3& direction = glm::vec3(0.0f, 0.0f, -1.0f)
        , float pitch_ = 0.0f
        , float yaw_ = -M_PI/2.0f
        , float speedFactor = 1.5f
        , float slowFactor = 1.1f) noexcept;

    BaseCamera& operator=(const BaseCamera&) = default;
    BaseCamera& operator=(BaseCamera&&) = default;
    BaseCamera(const BaseCamera&) = default;
    BaseCamera(BaseCamera&&) = default;
    virtual ~BaseCamera() {}
};
