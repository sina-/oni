#pragma once

#include <string_view>
#include <string>

#include <cereal/details/helpers.hpp>

namespace oni {
    template<class Archive>
    void
    saveHashedString(Archive &archive,
                     std::string_view name,
                     const oni::HashedString &data) {
        // TODO: Get the binarydata working so I don't have to create a new std string for each call!
//        auto dataBuffer = cereal::BinaryData(data.str.data(), data.str.size());
//        printf("size for %s : %lu\n", data.str.data(), data.str.size());
        auto dataBuffer = std::string(data.str);
        if (name.empty()) {
            archive(dataBuffer);
        } else {
            auto nameBuffer = cereal::BinaryData(name.data(), name.size());
            archive(nameBuffer, dataBuffer);
        }
    }

    template<class Archive>
    void
    loadHashedString(Archive &archive,
                     std::string_view name,
                     oni::HashedString &data) {
        auto buffer = std::string();
        //cereal::BinaryData<const char> buffer;

        if (name.empty()) {
            archive(buffer);
        } else {
            archive(name.data(), buffer);
        }

        auto result = oni::HashedString::makeFromStr(std::move(buffer));
        data.hash = result.hash;
        data.str = result.str;
    }

    template<class Archive, class ENUM>
    void
    loadEnum(Archive &archive,
             std::string_view name,
             ENUM &data) {
        std::string buffer{};
        if (name.empty()) {
            archive(buffer);
        } else {
            archive(name.data(), buffer);
        }
        auto hash_ = oni::HashedString::makeHashFromCString(buffer.data());
        // NOTE: After _init() hash will point to static storage so buffer is safe to go out of scope
        data._runtimeInit(hash_);
    }

    template<class Archive, class ENUM>
    void
    saveEnum(Archive &archive,
             std::string_view name,
             ENUM &data) {
        saveHashedString(archive, name, data.name);
    }
}
