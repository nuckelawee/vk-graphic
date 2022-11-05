#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class GLFWWindow {
    glfwWindow *pWindow;
    
    GLFWWindow() {}

    void Init();

    ~GLFWWindow();
};
