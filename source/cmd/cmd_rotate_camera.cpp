#include "cmd/cmd_rotate_camera.hpp"
#include "base_camera.hpp"

namespace cmd {

static bool valid(int x, int y) { return x != 0 || y != 0; }

RotateCamera::RotateCamera(BaseCamera* camera)
    : Mouse(CmdType::rotate_camera), camera_(camera) {}

void RotateCamera::Exicute() { 
    float pitch = -atan((deltaY_/sensity_));
    float yaw = -atan((deltaX_/sensity_));

    if(valid(deltaX_, deltaY_)) {
        camera_->Rotate(pitch, yaw);
        deltaX_ = 0;
        deltaY_ = 0;
    }
}

bool RotateCamera::operator==(const RotateCamera& other) const noexcept
{ return (camera_ == other.camera_); }
   
}
