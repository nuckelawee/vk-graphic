#pragma once

#include <vector>

class IEngine;
class Window;
class BaseCamera;
class Model;
namespace input { class Controller; }

class Application {

    IEngine *renderer_;
    Window *window_;
    BaseCamera *camera_;
    input::Controller *controller_;

private:
    void Update(std::vector<Model>& models);
    std::vector<Model> Init();

public:

    void Run();

    Application() = default;
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
};
