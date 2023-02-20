#include "vk/vk_vertex_binding.hpp"

namespace vk {

VkVertexInputBindingDescription VertexBinding::BindingDescription() {
    VkVertexInputBindingDescription bindingDescription {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex2D);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> VertexBinding::AttributeDescription() {
    size_t descriptionCount = 2;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(
        descriptionCount);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex2D, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex2D, color);
    return attributeDescriptions;
}

} // vk
