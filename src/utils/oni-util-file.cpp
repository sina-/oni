#include <oni-core/util/oni-util-file.h>

#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <cassert>

#include <oni-core/util/oni-util-structure.h>

namespace oni {
    std::string
    readFile(const std::string &path) {
        std::ifstream ifs(path.c_str());
        std::string content(std::istreambuf_iterator<char>(ifs.rdbuf()), std::istreambuf_iterator<char>());
        if (content.empty()) {
            assert(false);
        }
        return content;
    }

    void
    writeFile(const FilePath &fp,
              const std::stringstream &ss) {
        std::ofstream ofs(fp.getFullPath().data());
        if (!ofs.bad()) {
            ofs << ss.rdbuf();
        } else {
            assert(false);
        }
    }

    std::vector<FilePath>
    parseDirectoryTree(const FilePath &fp) {
        auto result = std::vector<FilePath>();
        for (auto &&p: std::filesystem::recursive_directory_iterator(fp.path)) {
            if (p.is_regular_file()) {
                const auto &path = p.path();
                if (path.has_parent_path()) {
                    auto filename = path.filename();
                    auto extension = std::string();
                    if (filename.has_extension()) {
                        extension = filename.extension();
                        filename.replace_extension("");
                    }
                    result.emplace_back(path.parent_path(), filename, extension.data());
                } else {
                    assert(false);
                }
            }
        }
        return result;
    }
}

