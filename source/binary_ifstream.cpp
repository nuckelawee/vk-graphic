#include "binary_ifstream.hpp"
#include <stdexcept>

BinaryIfstream::BinaryIfstream(const std::string& filepath) {
    file_ = fopen(filepath.c_str(), "rb");
    if(file_ == nullptr) {
        throw std::invalid_argument("Failed to open file [ " + filepath + " ]");
    }
}

BinaryIfstream::~BinaryIfstream() {
    fclose(file_);
}

void BinaryIfstream::Fread(void *buff, size_t size, size_t memberCount) const {
    if(fread(buff, size, memberCount, file_) != memberCount) {
        throw std::invalid_argument("Failed to read data from file");
    }
}
