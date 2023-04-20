#pragma once

#include "cmd_keyboard.hpp"

class BaseCamera;

namespace cmd {

enum class Side { forward, back, left, right, up, down };

class MoveCamera : public Keyboard {
    BaseCamera* camera_;
    Side side_;

public:

    MoveCamera(BaseCamera* camera, Side side);

    virtual void Exicute() override;

    bool operator==(const MoveCamera& other) const noexcept;

    virtual ~MoveCamera() = default;
};

} //cmd
