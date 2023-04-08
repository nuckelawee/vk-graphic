#include "application.hpp"
#include <memory>

int main() {
    std::unique_ptr<Application> app = std::make_unique<Application>();
    app->Run();
    return 0;
}
