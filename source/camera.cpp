#include "camera.hpp"
#include <chrono>

void Camera::Update(const AppSetting& appSetting) {
    float currentTime = appSetting.Statistic().CurTime();

    camera_.model = glm::rotate(glm::mat4(1.0f), currentTime * glm::radians(90.0f)//(M_PI/6)
        , glm::vec3(0.0f, 0.0f, 1.0f));
    camera_.view = glm::lookAt(position_, glm::vec3(0.0f, 0.0f, 0.0f)
        , glm::vec3(0.0f, 0.0f, 1.0f));
    camera_.projection = glm::perspective(glm::radians(45.0f)/*M_PI/4*/, appSetting.Width()
        / static_cast<float>(appSetting.Height()), 0.1f, 10.0f);
    camera_.projection[1][1] *= -1;
}

const MvpMatrix& Camera::Access() const {
    return camera_;
}

MvpMatrix& Camera::Access() {
    return camera_;
}
