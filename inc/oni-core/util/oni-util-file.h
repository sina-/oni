#pragma once

#include <string>
#include <vector>

#include <oni-core/util/oni-util-fwd.h>

namespace oni {
    std::string
    readFile(const std::string &path);

    // TODO: I don't really need a FilePath, but Directory path
    std::vector<FilePath>
    parseDirectoryTree(const FilePath &);
}
