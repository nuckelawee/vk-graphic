#include "cmd/cmd_exit.hpp"
#include "window.hpp"

namespace cmd {

Exit::Exit(Window* window) : Keyboard(CmdType::exit), window_(window) {}

void Exit::Exicute() {
    window_->Close();
}

bool Exit::operator==(const Exit& other) { return window_ == other.window_; }

}
