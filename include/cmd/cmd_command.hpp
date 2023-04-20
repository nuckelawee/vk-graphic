#pragma once

namespace cmd {

enum class CmdType { null, move_camera, rotate_camera, exit };

class Command {
    CmdType id_; 

protected:
    explicit Command(CmdType type);

public:

    Command();

    virtual void Exicute();
    CmdType Type() const noexcept;
    virtual void Extend(void* data);

    bool operator==(const Command& other) const noexcept;

    Command& operator=(const Command& other) = default;
    virtual ~Command() = default;
};

} //cmd
