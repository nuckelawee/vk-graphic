#include "input/input_controller.hpp"

#include "cmd/cmd_move_camera.hpp"
#include "cmd/cmd_rotate_camera.hpp"
#include "cmd/cmd_exit.hpp"

#include "base_camera.hpp"
#include "window.hpp"

namespace input {

void keyCallback(GLFWwindow* window, int key, int scancode
    , int action, int modes);

void cursorPosCallback(GLFWwindow* window, double x, double y);

void framebufferResize(GLFWwindow* window, int width, int height);

void Controller::SetCallbacks(GLFWwindow* window) noexcept {
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetFramebufferSizeCallback(window, framebufferResize);
}

void Controller::SetDefaultLayout(BaseCamera& camera, Window& window) {
    mouse_.SetCursorCommand(cmd::RotateCamera(&camera));

    keyboard_.SetCommand(cmd::Exit(&window), GLFW_KEY_ESCAPE);
    keyboard_.SetCommand(cmd::MoveCamera(&camera, cmd::Side::forward), GLFW_KEY_W);
    keyboard_.SetCommand(cmd::MoveCamera(&camera, cmd::Side::back), GLFW_KEY_S);
    keyboard_.SetCommand(cmd::MoveCamera(&camera, cmd::Side::left), GLFW_KEY_A);
    keyboard_.SetCommand(cmd::MoveCamera(&camera, cmd::Side::right), GLFW_KEY_D);
    keyboard_.SetCommand(cmd::MoveCamera(&camera, cmd::Side::up), GLFW_KEY_SPACE);
    keyboard_.SetCommand(cmd::MoveCamera(&camera, cmd::Side::down), GLFW_KEY_LEFT_SHIFT);
}

Controller::Controller(BaseCamera& camera, Window& window) noexcept {
    SetCallbacks(window.Access());
    glfwPollEvents();
    SetDefaultLayout(camera, window);
}

void Controller::KeyInput(int key, int scancode, int action, int modes) noexcept {
    keyCmd_ = keyboard_.KeyInput(key, scancode, action, modes);
}

void Controller::CursorPosition(int x, int y) noexcept {
    cursorCmd_ = mouse_.CursorPosition(x, y);
}

void Controller::MouseButton(int button, int action, int mods) noexcept {
    mouseCmd_ = mouse_.Button(button, action, mods);
}

void Controller::UpdateAll() noexcept {
    keyCmd_.Exicute();
    cursorCmd_.Exicute();
    mouseCmd_.Exicute();
}

void keyCallback(GLFWwindow* window, int key, int scancode
    , int action, int modes) {

    if(key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    input::Controller *controller = static_cast<input::Controller*>
        (glfwGetWindowUserPointer(window));

    controller->KeyInput(key, scancode, action, modes);
}

void cursorPosCallback(GLFWwindow* window, double x, double y) {
    input::Controller *controller = static_cast<input::Controller*>
        (glfwGetWindowUserPointer(window));

    controller->CursorPosition(static_cast<float>(x), static_cast<float>(y));
}

void framebufferResize(GLFWwindow* window, int width, int height) {
    
}

} // input
