#pragma once

#include <string>

namespace oni {
    struct FilePath {
        std::string value{};
    };

    static const FilePath INVALID_FILE_PATH = FilePath{""};

    struct ImageIndexFilePath {
        std::string value{};
    };

    std::string
    read_file(const std::string &path);
}
