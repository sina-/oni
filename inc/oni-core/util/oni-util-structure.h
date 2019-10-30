#pragma once

#include <string>

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    struct Hash {
        size value;

        inline constexpr bool
        operator!=(const Hash &other) const {
            return value != other.value;
        }

        inline constexpr bool
        operator==(const Hash &other) const {
            return value == other.value;
        }
    };

    struct FilePath {
        std::string path{};
        std::string name{};

        FilePath() = default;

        // NOTE: Not explicit on purpose.
        FilePath(std::string &&path_,
                 std::string &&name_) : path(std::move(path_)), name(std::move(name_)) { fixupPath(); }

        FilePath(const c8 *path_,
                 const c8 *name_) : path(path_), name(name_) { fixupPath(); }

        void
        descendInto(std::string_view dirName) {
            if (dirName.front() == '/') {
                dirName.remove_prefix(1);
            }
            path.append(dirName);
            fixupPath();
        }

        void
        fixupPath() {
            if (path.back() != '/') {
                path.append("/");
            }
        }

        std::string
        getFullPath() {
            return path + name;
        }
    };

    static const FilePath INVALID_FILE_PATH = FilePath{"__INVALID_FILE_PATH__", "__INVALID_FILE_NAME__"};

    struct ImageIndexFilePath : public FilePath {
        // NOTE: Path to the index.json, this file lists the all known image names and image filenames
    };

    struct EntityDefDirPath : public FilePath {
        // NOTE: Directory with data files that define entities
    };
}

namespace std {
    template<>
    class hash<oni::Hash> {
    public:
        constexpr size_t
        operator()(oni::Hash const &hs) const {
            return hs.value;
        }
    };
}
