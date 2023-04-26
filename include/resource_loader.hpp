#pragma once

#include <vector>
class ModelSet;
class Modelpath;

class ResourceLoader {
    constexpr static const char *missed_texture = "resources/missed_texture.tga";

public:

    static ModelSet LoadModel(std::vector<Modelpath>&& pathes) noexcept;
};
