#include "cmd/cmd_command.hpp"

namespace cmd {

Command::Command(CmdType type) : id_(type) {}
Command::Command() : id_(CmdType::null) {}

void Command::Exicute() {}

CmdType Command::Type() const noexcept { return id_; }

void Command::Extend(void *data) {}

bool Command::operator==(const Command& other) const noexcept
{ return id_ == other.id_; }

}
