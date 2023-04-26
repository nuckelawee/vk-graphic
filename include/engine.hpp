#pragma once

#include <vector>

class Window;
class BaseCamera;
class Model;

class IEngine {

public:

    virtual Window* Init() = 0; 
    virtual void Render(const std::vector<Model>& models) = 0;
    virtual std::vector<Model> Bind(ModelSet& set) = 0;

    virtual BaseCamera* CreateCamera() noexcept = 0;
    virtual void DestroyCamera(BaseCamera* camera) noexcept = 0;

    IEngine& operator=(const IEngine& engine) = delete;
    IEngine(const IEngine& engine) = delete;

    IEngine() = default;
    virtual ~IEngine() = default;
}; 

