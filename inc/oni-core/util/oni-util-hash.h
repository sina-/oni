#pragma once

#include <string_view>
#include <unordered_map>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/util/oni-util-structure.h>

#define DEFINE_STD_HASH_FUNCTIONS(CHILD)                    \
namespace std {                                             \
    template<>                                              \
    class hash<CHILD> {                                     \
    public:                                                 \
        constexpr size_t                                    \
        operator()(CHILD const &hs) const {                 \
            return hs.hash.value;                           \
        }                                                   \
                                                            \
        constexpr bool                                      \
        operator()(const CHILD &lhs,                        \
                   const CHILD &rhs) const {                \
            return lhs.hash == rhs.hash;                    \
        }};                                                 \
}

namespace oni {
    namespace detail {
        inline static constexpr u64 offset = 14695981039346656037ull;
        inline static constexpr u64 prime = 1099511628211ull;

        inline static std::unordered_map<Hash, std::string> knownStringStorage = {};
    }

    struct StringLiteral {
        // This has to be implicit so it won't collide with other implicit stiring literal constructor
        // to support constexpr construction of both:
        // 1) constexpr HashedString hashed = {"fooooo"};
        // 2) constexpr auto hashed = HashedString("foooo");
        constexpr StringLiteral(const c8 *curr) noexcept: str{curr} {}

        const c8 *str;
    };

    struct HashedString {
        std::string_view str{};
        Hash hash{};

        inline constexpr HashedString() = default;

        /**
         *  This version does not own the string. The use case in mind is string literals
         */
        template<size N>
        inline constexpr
        HashedString(const c8 (&value)[N]) noexcept: str(value), hash(makeHashFromLiteral(value)) {}

        /**
         * This version stores the str in internal storage
         */
        inline static HashedString
        makeFromStr(std::string &&value) {
            auto result = HashedString{};
            result.hash = _runtimeHash(value.data());
            auto knownStr = detail::knownStringStorage.find(result.hash);
            if (knownStr == detail::knownStringStorage.end()) {
                // TODO: The wording of emplace makes it not very obvious as if this will cause yet another construction
                // of the string :/
                auto storageStr = detail::knownStringStorage.emplace(result.hash, std::move(value)).first;
                result.str = {storageStr->second.data(), storageStr->second.size()};
            } else {
                result.str = {knownStr->second.data(), knownStr->second.size()};
                // TODO: Not sure how else I can make sure the original is invalid :/
                value.clear();
            }
            return result;
        }

        // TODO: I couldn't get the constructors for HashedString(std::string&&) and HashedString(const char*)
        //  work peacefully with
        //  constexpr string literal version of this code. So here we are with two factory functions that makes
        //  the intent explicit. Maybe I will keep it like this, but it might worth revisiting it in the future
        //  and trying to find a nicer interface.
        inline static HashedString
        makeFromCStr(const c8 *value) {
            auto result = HashedString{};
            result.hash = _runtimeHash(value);
            auto knownStr = detail::knownStringStorage.find(result.hash);
            if (knownStr == detail::knownStringStorage.end()) {
                auto storageStr = detail::knownStringStorage.emplace(result.hash, value).first;
                result.str = {storageStr->second.data(), storageStr->second.size()};
            } else {
                result.str = {knownStr->second.data(), knownStr->second.size()};
            }
            return result;
        }

        inline static Hash
        makeHashFromCString(StringLiteral value) noexcept {
            return _runtimeHash(value.str);
        }

        template<size N>
        inline static constexpr Hash
        makeHashFromLiteral(const c8 (&value)[N]) noexcept {
            return _staticHash(Hash{detail::offset}, value);
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
            return hash.value != 0 && !str.empty();
        }

        template<class Archive>
        void
        save(Archive &archive) const {
            // TODO: Not super happy about this, would it allocate?
            archive("name", std::string(str));
        }

        template<class Archive>
        void
        load(Archive &archive) {
            std::string buffer{};
            archive("name", buffer);
            auto result = makeFromStr(std::move(buffer));
            hash = result.hash;
            str = result.str;
        }

    private:
        inline static constexpr Hash
        _hash(const Hash partial,
              const c8 current) {
            // Fowler-Noll-Vo hashing
            return Hash{(partial.value ^ current) * detail::prime};
        }

        inline static constexpr Hash
        _staticHash(Hash partial,
                    const c8 *current) {
            return current[0] == 0 ? partial : _staticHash(_hash(partial, current[0]), current + 1);
        }

        inline static Hash
        _runtimeHash(std::string_view value) {
            auto result = Hash{detail::offset};
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

DEFINE_STD_HASH_FUNCTIONS(oni::HashedString)
