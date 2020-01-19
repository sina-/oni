#pragma once

#include <string>
#include <cassert>

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
        std::string extension{};

        FilePath() = default;

        // NOTE: Intentional implicit construction so that derived types can be constructed without need to name them
        FilePath(std::string &&path_) : path(std::move(path_)) { fixupPath(); }

        FilePath(std::string &&path_,
                 std::string &&name_,
                 std::string &&extension_) : path(std::move(path_)), name(std::move(name_)),
                                             extension(std::move(extension_)) { fixupPath(); }

        FilePath(const c8 *path_,
                 const c8 *name_,
                 const c8 *extension_) : path(path_), name(name_), extension(extension_) { fixupPath(); }

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
            if (!path.empty() && path.back() != '/') {
                path.append("/");
            }
            if (extension.front() == '.') {
                extension.erase(extension.cbegin());
            }
        }

        std::string
        getFullPath() const {
            auto result = std::string();
            if(!path.empty()) {
                result.append(path);
            }
            if(!name.empty()) {
                result.append(name);
            }
            if(!extension.empty()) {
                result.append(".");
                result.append(extension);
            }
            assert(!result.empty());
            return result;
        }
    };

    inline const FilePath INVALID_FILE_PATH = FilePath{"__INVALID_FILE_PATH__", "__INVALID_FILE_NAME__",
                                                       "__INVALID_EXTENSION__"};

    struct ImageIndexFilePath : public FilePath {
        // NOTE: Path to the index.json, this file lists the all known image names and image filenames
    };

    struct SoundIndexFilePath : public FilePath {
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
