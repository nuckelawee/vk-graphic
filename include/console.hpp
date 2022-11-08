#pragma once

#include <iostream>
#include <string>
#include <cstring>

#include "app_setting.hpp"

class App;

class Console {
    friend class App;

    static AppState ProccessInput(AppState state, const std::string& userInput);

    static void Input(AppSetting& setting);

    Console() = delete;
    ~Console() = delete; 
};
