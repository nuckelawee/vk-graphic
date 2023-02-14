#include "camera.hpp"

void Camera::Update() {
    float deltaTime = AppSetting::Get().statistic.DeltaTime();
    model = glm::rotate(glm::mat4(1.0f), deltaTime * (M_PI/6)
        , glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::lookAt(position, glm::vec3(0.0f, 0.0f, 0.0f)
        , glm::vec3(0.0f, 0.0f, 1.0f));
    projection = glm::perspective(M_PI/4, AppSetting::Get().Width()
        / AppSetting::Get().Height(), 0.1f, 10.0f);
    projection[1][1] *= -1;
}
