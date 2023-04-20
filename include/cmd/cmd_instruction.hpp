#pragma once

#include <memory>

namespace cmd {

class Command;

class Instruction {
    std::shared_ptr<Command> cmd_;
public:

    Instruction() noexcept;
    
    template<typename T>
    Instruction(const T& command) noexcept : cmd_(std::make_shared<T>(command)) {}

    void Exicute();
    void Extend(void* data);

    bool operator==(const Instruction& other) const;
    /*
    Instruction(Instruction&& other) = default;
    Instruction(const Instruction& cmd) = default;

    Instruction& operator=(Instruction&& other) = default;
    Instruction& operator=(const Instruction& cmd) = default;
    ~Instruction();
    */
};

} //cmd
