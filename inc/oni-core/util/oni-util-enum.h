#pragma once

#include <functional>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/util/oni-util-hash.h>
#include <oni-core/math/oni-math-function.h>

/* Criteria for a better enum type:
 * 1) minimal macro and template use
 * 2) simple io from and to string
 * 3) possibility to iterate over
 * 4) header only
 * 5) storage should require ~64-bit
 * 6) ::ENUM_NAME or something quite similar should be available on the enum
 * 7) easy to use in if statements
 */


#define ONI_ENUM_DEF_WITH_BASE(NAME, BASE, ...)                                                                                   \
    namespace detail{ inline constexpr BASE storage_##NAME [] = { __VA_ARGS__};}                                        \
    struct NAME : public oni::detail::EnumBase<                                                                         \
            sizeof(detail::storage_##NAME) / sizeof(BASE), BASE, detail::storage_##NAME> {                              \
       template<oni::i32 n>                                                                                             \
       inline static constexpr                                                                                          \
       NAME GET(const oni::c8 (&name)[n])                                                                               \
       { return {_get(name)};}                                                                                          \
};

#define ONI_ENUM_DEF(NAME, ...) ONI_ENUM_DEF_WITH_BASE(NAME, oni::Enum, __VA_ARGS__ )


namespace oni {
    struct Enum {
        oni::i32 id{0};
        oni::HashedString name{};

        inline constexpr bool
        operator==(const Enum &other) const {
            return other.id == id && other.name == name;
        }

        inline constexpr bool
        operator!=(const Enum &other) const {
            return other.id != id || other.name != name;
        }

        using type = decltype(id);
    };

    namespace detail {
        template<oni::i32 N, class BASE, auto v>
        struct EnumBase : public BASE {
            inline static constexpr auto
            begin() {
                return storage;
            }

            inline static constexpr auto
            end() {
                return storage + N;
            }

            inline bool
            valid() {
                return BASE::name != INVALID.name;
            }

            // NOTE: It is possible to have duplicate ids but different names!
            inline constexpr bool
            operator==(const BASE &other) const {
                return other.id == BASE::id && other.name == BASE::name;
            }

            inline constexpr bool
            operator!=(const BASE &other) const {
                return other.id != BASE::id || other.name != BASE::name;
            }

            // NOTE: It is possible to have duplicate ids but different names!
            inline constexpr bool
            operator==(const HashedString &other) const {
                return other.hash == BASE::name.hash;
            }

            inline constexpr bool
            operator!=(const HashedString &other) const {
                return other.hash != BASE::name.hash;
            }

            inline static constexpr oni::i32
            size() {
                return N;
            }

            // NOTE: Implicit so that this class can be used in switch statements
            // TODO: Sad I can't really have it like this. With this one, you could compare two different
            // enums based on id! even though those ids have nothing to do with each other. Which beats the
            // whole point of type system with my enum design.
//            inline constexpr operator i32() const {
//                return BASE::id;
//            }

            template<class Out, class Adaptor>
            static inline Out *
            adapt(const Adaptor &adaptor) {
                static bool init = true;
                static auto result = std::array<Out, N>();
                if (init) {
                    for (oni::i32 i = 0; i < N; ++i) {
                        result[i] = adaptor(storage[i]);
                    }
                    init = false;
                }
                return result.data();
            }

            void
            runtimeInit(const oni::Hash hash) {
                for (oni::i32 i = 0; i < N; ++i) {
                    if (storage[i].name.hash == hash) {
                        BASE::name = storage[i].name;
                        BASE::id = storage[i].id;
                        return;
                    }
                }
                BASE::id = INVALID.id;
                BASE::name = INVALID.name;
            }

//            inline static constexpr BASE
//            make(const Hash &value) {
//                return _find(value, 0, storage);
//            }

        protected:
            template<oni::i32 n>
            static inline constexpr BASE
            _get(const oni::c8 (&name_)[n]) {
                return _find(oni::HashedString::makeHashFromLiteral(name_), 0, storage);
            }

        private:
            // TODO: How can I make this better, the diagnosis error is misleading
            inline static constexpr BASE
            _notFound() {
                throw std::logic_error("Invalid value specified for the enum");
                return INVALID;
            }

            inline static constexpr BASE
            _find(const oni::Hash hash,
                  oni::i32 i,
                  const BASE *candidate) {
                return (candidate->name.hash == hash) ? *candidate :
                       ((i == N) ? _notFound() : _find(hash, i + 1, candidate + 1));
            }

        private:
            static inline const constexpr BASE *storage = v;
            static inline const constexpr BASE INVALID = {0, "__INVALID__"};
        };
    }
}

namespace std {
    template<>
    class hash<oni::Enum> {
    public:
        constexpr size_t
        operator()(oni::Enum const &hs) const {
            return hs.name.hash.value;
        }

        constexpr bool
        operator()(const oni::Enum &lhs,
                   const oni::Enum &rhs) const {
            return lhs.name.hash == rhs.name.hash;
        }
    };
}
