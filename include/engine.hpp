#pragma once

class Window;
class BaseCamera;

class IEngine {

public:

    virtual Window* Init() = 0; 
    virtual void Update() = 0;
    virtual BaseCamera* CreateCamera() noexcept = 0;

    IEngine& operator=(const IEngine& engine) = delete;
    IEngine(const IEngine& engine) = delete;

    IEngine() = default;
    virtual ~IEngine() = default;
}; 

