#pragma once

class IEngine;
class Window;
class BaseCamera;
namespace input { class Controller; }

class Application {

    IEngine *renderer_;
    Window *window_;
    BaseCamera *camera_;
    input::Controller *controller_;

private:
    void Update();
    void Init();

public:

    void Run();

    Application() = default;
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
};
