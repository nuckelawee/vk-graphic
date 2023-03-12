#pragma once

enum FileResult {
    FILE_SUCCESS = 0,
    FILE_ERROR_OPEN_FILE = -1,
    FILE_ERROR_TO_READ = -4,
    FILE_ERROR_MEMORY_ALLOCATION = -6,
    FILE_TGA_ERROR_NO_IMAGE = -2,
    FILE_TGA_ERROR_NOT_SUPPORT = -3,
    FILE_TGA_ERROR_FILE_IS_CORRUPTED = -5,
    FILE_TGA_ERROR_NOT_TGA = -7,
};
