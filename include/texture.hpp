#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

using byte = unsigned char;

struct Texture {
    uint64_t size = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    byte bytesPerPixel = 0;
    VkFormat format = VK_FORMAT_UNDEFINED; 
    byte *pixels = nullptr;   

    Texture() {}
    Texture(const Texture& t);
    Texture(Texture&& t);
    ~Texture();

    Texture& operator=(const Texture& t);
    Texture& operator=(Texture&& t);

};
