#include "cmd/cmd_move_camera.hpp"
#include "base_camera.hpp"

namespace cmd {

MoveCamera::MoveCamera(BaseCamera* camera, Side side)
        : Keyboard(CmdType::move_camera), camera_(camera), side_(side) {}

void MoveCamera::Exicute() {
    switch(side_) {
    case Side::forward:
        camera_->MoveForward();
        break;
    case Side::back:
        camera_->MoveBack();
        break;
    case Side::left:
        camera_->MoveLeft();
        break;
    case Side::right:
        camera_->MoveRight();
        break;
    case Side::up:
        camera_->MoveUp();
        break;
    case Side::down:
        camera_->MoveDown();
        break;
    }
}

bool MoveCamera::operator==(const MoveCamera& other) const noexcept {
    return (camera_ == other.camera_ && side_ == other.side_);
}

} //cmd
