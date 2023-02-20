#include "camera.hpp"
#include <chrono>

void Camera::Update(const AppSetting& appSetting) {
//    float currentTime = appSetting.Statistic().CurTime();
    float deltaTime = appSetting.Statistic().DeltaTime();

    position_ += speed_ * deltaTime;

    camera_.model = glm::rotate(glm::mat4(1.0f), 0.0f//currentTime * (M_PI/6)
        , glm::vec3(0.0f, 0.0f, 1.0f));

    camera_.model = glm::mat4(1.0f);

    camera_.view = glm::lookAt(position_, position_ + glm::vec3(0.0f, 0.0f, -1.0f)
        , glm::vec3(0.0f, 1.0f, 0.0f));
    camera_.projection = glm::mat4(1.0f);

    camera_.projection = glm::perspective(static_cast<float>(M_PI/2), appSetting.Width()
        / static_cast<float>(appSetting.Height()), 0.1f, 100.0f);

    speed_ *= slowFactor * deltaTime;
}

void Camera::Move(const glm::vec3& direct) {
    speed_ += direct * speedFactor;
}

const MvpMatrix& Camera::Access() const {
    return camera_;
}

MvpMatrix& Camera::Access() {
    return camera_;
}
