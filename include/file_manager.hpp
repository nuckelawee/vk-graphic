#pragma once

#include <fstream>

#include "error_manager.hpp"

class FileManager {

public:

    static std::string ReadFile(const std::string&, std::ios_base::openmode);
};
