#include "cmd/cmd_instruction.hpp"
#include "cmd/cmd_command.hpp"

namespace cmd {

Instruction::Instruction() noexcept : cmd_(std::make_shared<Command>()) {}

/*
template<typename T>
Instruction::Instruction(const T& command) noexcept
*/

void Instruction::Exicute() { cmd_->Exicute(); }
void Instruction::Extend(void* data) { cmd_->Extend(data); }

bool Instruction::operator==(const Instruction& other) const { return cmd_ == cmd_; }

} //cmd
