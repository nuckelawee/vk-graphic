#pragma once

#include <string>
#include <cstdio>

class BinaryIfstream {
    FILE *file_;

public:

    BinaryIfstream(const std::string& filepath);

    void Fread(void *buff, size_t size, size_t memberCount) const;

    ~BinaryIfstream();

    BinaryIfstream(BinaryIfstream&&) = default;
    BinaryIfstream& operator=(BinaryIfstream&&) = default;

    BinaryIfstream(const BinaryIfstream&) = delete;
    BinaryIfstream& operator=(const BinaryIfstream&) = delete;
};
