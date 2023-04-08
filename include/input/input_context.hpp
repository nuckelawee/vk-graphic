#pragma once

#include <vector>

namespace input {

enum class ContextType { camera, unknown };

class Context {
    ContextType type_ = ContextType::unknown;
    void* context_ = nullptr;

public:

    Context() {}
    Context(ContextType type, void* context);
    
    void UpdateMouseButtons(int button) noexcept;
    void UpdateMousePosition(float deltaX, float deltaY) noexcept;

    void UpdateKeys(const std::vector<bool>& keys) noexcept;

    void Switch(ContextType type, void* context) noexcept;

private:

    void MoveCamera(const std::vector<bool>& keys) noexcept;
    void RotateCamera(float deltaXPos, float deltaYPos) noexcept;

};

} // input
