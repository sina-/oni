#pragma once

#include <functional>

namespace oni {
    using Hash = size;

    static Hash
    hashString(const std::string &value) {
        static std::hash<std::string> func;
        return func(value);
    }

    template<size N>
    static constexpr Hash
    hashString(const c8 (&value)[N]) {
        std::hash<const c8 (&)[N]> func;
        return func(value);
    }

    // TODO: I really need to do something about all these std::strings that are allocating memory in runtime
    // the general concept of constexpr hashing should help with most of these allocations.
    static Hash
    hashString(const c8 *value) {
        auto string = std::string(value);
        return hashString(string);
    }

    struct HashedString {
        std::string data{};
        Hash hash{};

        HashedString() = default;

        // TODO: It would be nice to has a constexpr constructor for this one!
        // TODO: The base of issue is that I want my HashedString to support constexpr chars and
        // runtime chars! Right now this only supports runtime chars.
        explicit HashedString(const c8 *value) noexcept: data(value), hash(hashString(data)) {}

        explicit HashedString(std::string &&value) noexcept: data(std::move(value)), hash(hashString(data)) {}

        template<std::size_t N>
        constexpr
        HashedString(const c8 (&value)[N]) noexcept;

//        explicit HashedString(std::string_view value) noexcept: data(value), hash(hashString(data)) {}

        // TODO: Hmmm don't really want to create one constructor per N and include this massive header in
        // every fucking place!
        // NOTE: This matches {"random-string"} type of initializer-list
//        template<std::size_t N>
//        HashedString(const c8 (&value)[N]) noexcept: data(value), hash(hashString(data)) {}

        bool
        operator==(const HashedString &other) const {
            return hash == other.hash;
        }

        template<class Archive>
        void
        save(Archive &archive) const {
            // NOTE: hash is not serialized!
            archive(data);
        }

        template<class Archive>
        void
        load(Archive &archive) {
            archive(data);
            hash = hashString(data);
        }
    };
}