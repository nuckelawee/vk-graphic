#pragma once

#include "cmd_mouse.hpp"

class BaseCamera;

namespace cmd {

class RotateCamera : public Mouse {
    BaseCamera* camera_;

public:
    explicit RotateCamera(BaseCamera* camera);

    virtual void Exicute() override;

    bool operator==(const RotateCamera& other) const noexcept;
    virtual ~RotateCamera() = default;
};

}//cmd
