#include <oni-core/util/oni-util-file.h>

#include <fstream>
#include <string>


std::string
oni::readFile(const std::string &path) {
    std::ifstream ifs(path.c_str());
    std::string content(std::istreambuf_iterator<char>(ifs.rdbuf()), std::istreambuf_iterator<char>());
    if (content.empty())
        throw std::runtime_error("Empty file " + path);
    return content;
}
