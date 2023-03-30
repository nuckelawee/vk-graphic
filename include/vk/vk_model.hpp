#pragma once

#include <cstdint>
#include <string>

namespace vk {

class ModelStorage;

struct ModelInfo {
    std::string meshpath;
    std::string texturepath;
};

class Model {
    friend class ModelStorage;

    uint32_t descriptor_;
    bool visible_ = true;  

public:

    void Draw();

    Model(uint32_t desc) : descriptor_(desc) {}
    ~Model() {}

};

} //vk
