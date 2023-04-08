#include "base_camera.hpp"
#include "timer.hpp"
#include "global_settings.hpp"

void BaseCamera::UpdateMatrices() noexcept {
    float deltaTime = Timer::GetInstance().DeltaTime();
    position_ += speed_ * deltaTime;

    camera_.model = glm::rotate(glm::mat4(1.0f), 0.0f
        , glm::vec3(0.0f, 0.0f, 1.0f));

    camera_.view = glm::lookAt(position_, position_ + direction_, up_);

    camera_.projection = glm::perspective(static_cast<float>(M_PI/4)
        , GlobalSettings::GetInstance().ScreenWidth()
        / static_cast<float>(GlobalSettings::GetInstance().ScreenHeight())
        , 0.1f, 100.0f);

    camera_.projection[1][1]  *= -1;

    speed_ *= slowFactor_ * deltaTime;
}

void BaseCamera::Update() noexcept {
    if(needUpdate_) {
        UpdateMatrices();
        needUpdate_ = false;
    }
}

void BaseCamera::Rotate(float deltaPitch, float deltaYaw) noexcept {

    pitch_ += deltaPitch; 
    yaw_ += deltaYaw;

    direction_.x = cos(pitch_) * cos(yaw_);
    direction_.y = -sin(pitch_);
    direction_.z = cos(pitch_) * sin(yaw_);
    direction_ = glm::normalize(direction_);
    needUpdate_ = true;
}

void BaseCamera::MoveForward() noexcept {
    speed_ += direction_ * speedFactor_;
    needUpdate_ = true;
}
void BaseCamera::MoveBack() noexcept {
    speed_ -= direction_ * speedFactor_;
    needUpdate_ = true;
}
void BaseCamera::MoveRight() noexcept {
    speed_ += glm::cross(direction_, up_) * speedFactor_;
    needUpdate_ = true;
}
void BaseCamera::MoveLeft() noexcept {
    speed_ += glm::cross(up_, direction_) * speedFactor_;
    needUpdate_ = true;
}
void BaseCamera::MoveUp() noexcept {
    speed_ += up_ * speedFactor_;
    needUpdate_ = true;
}
void BaseCamera::MoveDown() noexcept {
    speed_ -= up_ * speedFactor_;
    needUpdate_ = true;
}

BaseCamera::BaseCamera(const glm::vec3& position, const glm::vec3& speed
    , const glm::vec3& direction, float pitch , float yaw
    , float speedFactor, float slowFactor)
    : position_(position), speed_(speed), direction_(direction)
    , pitch_(pitch), yaw_(yaw)
    , speedFactor_(speedFactor), slowFactor_(slowFactor) {}
    
