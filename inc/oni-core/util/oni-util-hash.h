#pragma once

#include <string_view>

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    using Hash = size;

    struct HashedString {
        std::string_view str{};
        Hash hash{};

        static inline constexpr u64 offset = 14695981039346656037ull;
        static inline constexpr u64 prime = 1099511628211ull;

        constexpr HashedString() = default;

        constexpr HashedString(std::string_view data_,
                               Hash hash_) noexcept: str(data_), hash(hash_) {}

        template<size N>
        explicit constexpr
        HashedString(const c8 (&value)[N]) noexcept {
            str = value;
            hash = _getHash(value);
        }

        explicit
        HashedString(std::string_view value) noexcept {
            str = value;
            hash = _getHash(str);
        }

        static constexpr HashedString
        view(const std::string &value) noexcept {
            return {value.data(), _getHash(value)};
        }

        bool
        operator!=(const HashedString &other) const {
            return hash != other.hash;
        }

        bool
        operator==(const HashedString &other) const {
            return hash == other.hash;
        }

        bool
        valid() const {
            return hash != 0 && !str.empty();
        }

    private:
        static constexpr Hash
        _hash(Hash partial,
              const c8 current) {
            // Fowler-Noll-Vo hashing
            return (partial ^ current) * prime;
        }

        template<size N>
        static constexpr Hash
        _getHash(const c8 (&value)[N]) {
            auto result = offset;
            for (Hash i = 0; i < N; ++i) {
                result = _hash(result, value[i]);
            }
            return result;
        }

        static constexpr Hash
        _getHash(std::string_view value) {
            auto result = offset;
            for (auto &&v : value) {
                result = _hash(result, v);
            }
            // Because string_view is not null terminated!
            return _hash(result, '\0');
        }
    };

}