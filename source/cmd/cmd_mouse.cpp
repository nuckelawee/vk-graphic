#include "cmd/cmd_mouse.hpp"

namespace cmd {

void Mouse::Extend(void* data) {
    float* coords = static_cast<float*>(data);
    x_ = coords[0];
    y_ = coords[1];
    deltaX_ = coords[2];
    deltaY_ = coords[3];
}

}
