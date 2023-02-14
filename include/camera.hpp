#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

public:

    void Update();

};
