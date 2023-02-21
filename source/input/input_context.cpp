#include "input/input_context.hpp"

namespace input {

void Context::UpdateMouseButtons(int button) {

}


void Context::UpdateMousePosition(float deltaX, float deltaY) {
    if(type_ == CONTEXT_TYPE_CAMERA) {
        RotateCamera(deltaX, deltaY);  
    } 
}

void Context::UpdateKeys(const bool *pKeys) {
     if(type_ == CONTEXT_TYPE_CAMERA) {
        MoveCamera(pKeys);
    }     
}

void Context::RotateCamera(float deltaXPos, float deltaYPos) {
    Camera *pCamera = static_cast<Camera*>(pContext_);
    float sensity = 500;
    float yaw = -atan((deltaXPos/sensity));
    float pitch = -atan((deltaYPos/sensity));
    //std::cout << "deltaYpos " << deltaYPos << " delta pitch " << pitch << '\n';

    pCamera->Rotate(pitch, yaw);
}

void Context::MoveCamera(const bool *pKeys) {
    Camera *pCamera = static_cast<Camera*>(pContext_); 
    glm::vec3 direct(0);

    if(pKeys[GLFW_KEY_W]) { pCamera->MoveForward(); }
    if(pKeys[GLFW_KEY_S]) { pCamera->MoveBack(); }
    if(pKeys[GLFW_KEY_A]) { pCamera->MoveLeft(); }
    if(pKeys[GLFW_KEY_D]) { pCamera->MoveRight(); }
    if(pKeys[GLFW_KEY_SPACE]) { pCamera->MoveUp(); }
    if(pKeys[GLFW_KEY_LEFT_SHIFT]) { pCamera->MoveDown(); }
}

void Context::Switch(void *pContext, ContextType type) {
    pContext_ = pContext;
    type_ = type;
}

}
