#include <fstream>

#include <oni-core/utils/file.h>

std::string
oni::read_file(const std::string &path) {
    std::ifstream ifs(path.c_str());
    std::string content(std::istreambuf_iterator<char>(ifs.rdbuf()), std::istreambuf_iterator<char>());
    if (content.empty())
        throw std::runtime_error("Empty file " + path);
    return content;
}
