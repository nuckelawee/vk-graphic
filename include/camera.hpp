#pragma once

#include "vk/vk_descriptor_set.hpp"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

class Camera {

    MvpMatrix camera_;
    glm::vec3 position_ = glm::vec3(2.0f, 2.0f, 2.0f);

public:

    void Update(const AppSetting& appSetting);

    const MvpMatrix& Access() const;
    MvpMatrix& Access();

    Camera() {}
    ~Camera() {}

};
