#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>

namespace vk {

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription BindingDescription();
    static std::vector<VkVertexInputAttributeDescription> AttributeDescription();
};

}
