#include "console.hpp"

void Console::Input(AppSetting& setting) {
    while(setting.State() != APP_STATE_TERMINATE) {
        std::string userInput;
    //    std::cin >> userInput;
    //    setting.ChangeState(ProccessInput(setting.State(), userInput));
    }
}

AppState Console::ProccessInput(AppState state, const std::string& userInput) {
    if(strcmp(userInput.c_str(), "start") == 0)
    { return APP_STATE_WORK; }
    if(strcmp(userInput.c_str(), "stop") == 0)
    { return APP_STATE_STOP; }
    if(strcmp(userInput.c_str(), "terminate") == 0)
    { return APP_STATE_TERMINATE; }
    std::cout << "Unkown command\n";
    return state;
}
