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
        auto buffer = std::string(data.str);
        if (name.empty()) {
            archive(buffer);
        } else {
            // auto nameBuffer = cereal::BinaryData(name.data(), name.size());
            archive(name.data(), buffer);
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
}
