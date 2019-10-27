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
        inline constexpr
        HashedString(const c8 (&value)[N]) noexcept: str(value), hash(get(value)) {}

        // TODO: Can this be constexpr?
        explicit
        HashedString(std::string_view value) noexcept : str(value), hash(_runtimeHash(str)) {}

        inline static HashedString
        view(const std::string &value) noexcept {
            return {value.data(), _runtimeHash(value)};
        }

        template<size N>
        inline static constexpr Hash
        get(const c8 (&value)[N]) noexcept {
            return _staticHash(offset, value);
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
        inline static constexpr Hash
        _hash(const Hash partial,
              const c8 current) {
            // Fowler-Noll-Vo hashing
            return (partial ^ current) * prime;
        }

        inline static constexpr Hash
        _staticHash(Hash partial,
                    const c8 *current) {
            return current[0] == 0 ? partial : _staticHash(_hash(partial, current[0]), current + 1);
        }

        inline static Hash
        _runtimeHash(std::string_view value) {
            auto result = offset;
            for (auto &&v : value) {
                if (v == 0) {
                    return result;
                }
                result = _hash(result, v);
            }
            return result;
        }
    };
}