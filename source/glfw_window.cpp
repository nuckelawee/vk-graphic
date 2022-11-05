#include "glfw_window.hpp"

GLFWWindow::Init() {
    if(glfwInit() == 0) {
        


GLFWWindow::~GLFWWindow() {
    glfwDestroyWindow();
    glfwTerminate();
}
