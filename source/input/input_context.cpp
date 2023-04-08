#include "input/input_context.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>

#include "base_camera.hpp"

namespace input {

Context::Context(ContextType type, void* context) : type_(type), context_(context) {}

void Context::UpdateMouseButtons(int button) noexcept {

}


void Context::UpdateMousePosition(float deltaX, float deltaY) noexcept {
    if(type_ == ContextType::camera) {
        RotateCamera(deltaX, deltaY);  
    } 
}

void Context::UpdateKeys(const std::vector<bool>& keys) noexcept {
     if(type_ == ContextType::camera) {
        MoveCamera(keys);
    }     
}

void Context::RotateCamera(float deltaXPos, float deltaYPos) noexcept {
    BaseCamera *camera = static_cast<BaseCamera*>(context_);
    float sensity = 500;
    float yaw = -atan((deltaXPos/sensity));
    float pitch = -atan((deltaYPos/sensity));

    camera->Rotate(pitch, yaw);
}

void Context::MoveCamera(const std::vector<bool>& keys) noexcept {
    BaseCamera *camera = static_cast<BaseCamera*>(context_); 
    glm::vec3 direct(0);

    if(keys[GLFW_KEY_W]) { camera->MoveForward(); }
    if(keys[GLFW_KEY_S]) { camera->MoveBack(); }
    if(keys[GLFW_KEY_A]) { camera->MoveLeft(); }
    if(keys[GLFW_KEY_D]) { camera->MoveRight(); }
    if(keys[GLFW_KEY_SPACE]) { camera->MoveUp(); }
    if(keys[GLFW_KEY_LEFT_SHIFT]) { camera->MoveDown(); }
}

void Context::Switch(ContextType type, void* context) noexcept {
    type_ = type;
    context_ = context;
}

}
