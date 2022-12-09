#include "file_manager.hpp"

std::string FileManager::ReadFile(const std::string& filepath

    , std::ios_base::openmode mode) {
    std::ifstream file(filepath, mode);
    if(file.is_open() == false) {
        ErrorManager::Validate(WARNING, "Failed to open file ( "
            + filepath + " ) ", "File reading");
        return "";
    }
    size_t fileSize = static_cast<size_t>(file.tellg());
    std::string code;
    code.resize(fileSize);
    file.seekg(0);
    file.read(code.data(), fileSize);

    file.close();
    return code;
}
