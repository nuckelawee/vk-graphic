#include "camera.hpp"

void Camera::Update(const AppSetting& appSetting) {
    float deltaTime = appSetting.Statistic().DeltaTime();

    position_ += speed_ * deltaTime;

    camera_.model = glm::rotate(glm::mat4(1.0f), 0.0f
        , glm::vec3(0.0f, 0.0f, 1.0f));

    camera_.model = glm::mat4(1.0f);

    camera_.view = glm::lookAt(position_, position_ + direction_, up);
    camera_.projection = glm::mat4(1.0f);

    camera_.projection = glm::perspective(static_cast<float>(M_PI/4), appSetting.Width()
        / static_cast<float>(appSetting.Height()), 0.1f, 100.0f);

    speed_ *= slowFactor * deltaTime;
}

void Camera::Rotate(float deltaPitch, float deltaYaw) {
    pitch_ += deltaPitch; 
    yaw_ += deltaYaw;

    direction_.x = cos(pitch_) * cos(yaw_);
    direction_.y = sin(pitch_);
    direction_.z = cos(pitch_) * sin(yaw_);
    direction_ = glm::normalize(direction_);
}

void Camera::MoveForward() {
    speed_ += direction_ * speedFactor;
}
void Camera::MoveBack() {
    speed_ -= direction_ * speedFactor;
}
void Camera::MoveRight() {
    speed_ += glm::cross(direction_, up) * speedFactor;
}
void Camera::MoveLeft() {
    speed_ += glm::cross(up, direction_) * speedFactor;
}
void Camera::MoveUp() {
    speed_ -= up * speedFactor;
}
void Camera::MoveDown() {
    speed_ += up * speedFactor;
}


const MvpMatrix& Camera::Access() const {
    return camera_;
}

MvpMatrix& Camera::Access() {
    return camera_;
}
