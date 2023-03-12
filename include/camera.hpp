#pragma once

#include "vk/vk_descriptor_set.hpp"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

class Camera {

    MvpMatrix camera_;
    glm::vec3 position_ = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 speed_ = glm::vec3(0.0f, 0.0f, 0.0f); 
    glm::vec3 direction_ = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    const float speedFactor = 1.5f;
    const float slowFactor = 1.1f;

    float pitch_ = 0.0f;
    float yaw_ = -(M_PI/2.0f);

public:

    void Update(const AppSetting& appSetting);

    void Rotate(float deltaPitch, float deltaYaw);

    const MvpMatrix& Access() const;
    MvpMatrix& Access();

    void MoveForward();
    void MoveBack();
    void MoveRight();
    void MoveLeft();
    void MoveUp();
    void MoveDown();

    Camera() {}
    ~Camera() {}

};
