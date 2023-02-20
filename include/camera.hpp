#pragma once

#include "vk/vk_descriptor_set.hpp"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

class Camera {

    MvpMatrix camera_;
    glm::vec3 position_ = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 speed_ = glm::vec3(0.0f, 0.0f, 0.0f); 
    const float speedFactor = 2.5f;
    const float slowFactor = 1.1f;

public:

    void Update(const AppSetting& appSetting);

    void Move(const glm::vec3& direct);

    const MvpMatrix& Access() const;
    MvpMatrix& Access();

    Camera() {}
    ~Camera() {}

};
