#pragma once

#include "app_setting.hpp"
#include "camera.hpp"

namespace input {

class Context {
    ContextType type_ = CONTEXT_TYPE_UNKNOWN;
    void *pContext_ = nullptr;

public:
    
    void UpdateMouseButtons(int button);
    void UpdateMousePosition(float x, float y, float deltaX, float deltaY);

    void UpdateKeys(const bool *pKeys);

    void Switch(void *pContext, ContextType type);

private:

    void MoveCamera(const bool *pKeys);

public:

    Context() {}
    ~Context() {}

};

} // input
