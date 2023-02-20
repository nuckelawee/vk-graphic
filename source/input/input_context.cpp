#include "input/input_context.hpp"

namespace input {

void Context::UpdateMouseButtons(int button) {

}


void Context::UpdateMousePosition(float x, float y, float deltaX, float deltaY) {

}

void Context::UpdateKeys(const bool *pKeys) {
     if(type_ == CONTEXT_TYPE_CAMERA) {
        MoveCamera(pKeys);
    }     
}

void Context::MoveCamera(const bool *pKeys) {
    Camera *pCamera = static_cast<Camera*>(pContext_); 
    glm::vec3 direct(0);

    if(pKeys[GLFW_KEY_W]) { direct.z = -1; }
    if(pKeys[GLFW_KEY_S]) { direct.z = 1; }
    if(pKeys[GLFW_KEY_A]) { direct.x = -1; }
    if(pKeys[GLFW_KEY_D]) { direct.x = 1; }
    if(pKeys[GLFW_KEY_SPACE]) { direct.y = -1; }
    if(pKeys[GLFW_KEY_LEFT_SHIFT]) { direct.y = 1; }

    pCamera->Move(direct);
}

void Context::Switch(void *pContext, ContextType type) {
    pContext_ = pContext;
    type_ = type;
}

}
