#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>

namespace vk {

struct Vertex3D {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 texPos;
};

class VertexBinding {
public:
    static VkVertexInputBindingDescription BindingDescription();
    static std::vector<VkVertexInputAttributeDescription> AttributeDescription();
};

}
