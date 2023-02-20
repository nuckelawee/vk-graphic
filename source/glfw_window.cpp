#include "glfw_window.hpp"

void GlfwWindow::CreateWindow(input::Controller& controller) {
    ErrorManager::Validate(Error(UNSOLVABLE, glfwInit() == 0)
        , "GLFW library failed to init", "GLFW window creation");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    pWindow_ = glfwCreateWindow(setting_.Width(), setting_.Height()
        , setting_.Application().c_str(), nullptr, nullptr);

    ErrorManager::Validate(Error(UNSOLVABLE, pWindow_ == nullptr)
        , "GLFW window creation failed", "GLFW window creation");

    glfwSetWindowUserPointer(pWindow_, &controller);
    glfwSetKeyCallback(pWindow_, KeyCallback);
    glfwSetFramebufferSizeCallback(pWindow_, FramebufferResize);
    //glfwSwapInterval(1);
}

VkResult GlfwWindow::Create(const vk::Instance& instance) {
    VkResult result = glfwCreateWindowSurface(instance.Access(), pWindow_
        , nullptr, &surface_);
    ErrorManager::Validate(result, "GLFW surface creation");
    return result;
}

vk::SurfaceDetails GlfwWindow::Capabilities(const VkPhysicalDevice& gpu) const {
    vk::SurfaceDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface_
        , &details.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface_, &formatCount, nullptr);
    if(formatCount > 0) { 
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface_, &formatCount
            , details.formats.data());
    }

    uint32_t presentCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface_, &presentCount
        , nullptr);
    if(presentCount > 0) {
        details.presentModes.resize(presentCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface_, &presentCount
            , details.presentModes.data());
    } 

    return details;
}

void GlfwWindow::KeyCallback(GLFWwindow *pWindow, int key, int scancode
    , int action, int modes) {
    
    input::Controller *pController = static_cast<input::Controller*>
        (glfwGetWindowUserPointer(pWindow));

    pController->KeyInput(key, scancode, action, modes);

}

void GlfwWindow::CursorPos(GLFWwindow *pWindow, float x, float y) {
    input::Controller *pController = static_cast<input::Controller*>
        (glfwGetWindowUserPointer(pWindow));

    pController->CursorPosition(x, y);
}

void GlfwWindow::FramebufferResize(GLFWwindow *pWindow, int width, int height) {
    
}

void GlfwWindow::CloseWindow() {
    glfwSetWindowShouldClose(pWindow_, GLFW_TRUE);
}

bool GlfwWindow::ShouldClosed() const {
    return glfwWindowShouldClose(pWindow_);
}

GlfwWindow::~GlfwWindow() {
    glfwDestroyWindow(pWindow_);
    glfwTerminate();
}
