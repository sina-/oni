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
        std::string value{};
    };

    static const FilePath INVALID_FILE_PATH = FilePath{"__INVALID_FILE_PATH__"};

    struct ImageIndexFilePath : public FilePath {
        // NOTE: Path to the index.json, this file lists the all known image names and image filenames
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
