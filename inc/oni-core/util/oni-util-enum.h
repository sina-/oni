#pragma once

#include <functional>
#include <sstream>

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

/*
 * Current issues:
 * 1) Renaming and enum string is a pain in the ass
 * 2) GET function is slow down compilation. It requires calculating the hash for every string lookup
 * 3) What is the point of ID? Hash is ID enough! Usage-wise it is confusing, should users stick to
 * ID field or use hash? ID values could include duplicates, which is confusing. Can I just drop ID field?
 *
 * Improvements:
 * 1) Can I merge the concept of enum class with what I have? I once tried auto generating strings from
 * enum values, but that will still be slow to compile, probably not as slow as this one. Maybe I can
 * even define the string and the enum at the same time... hmmm..
 */


#define ONI_ENUM_DEF_WITH_BASE(NAME, BASE, ...)                                                                         \
    namespace oni_detail{ inline constexpr BASE storage_##NAME [] = { __VA_ARGS__};}                                    \
    struct NAME : public oni::oni_detail::EnumBase<                                                                     \
            sizeof(oni_detail::storage_##NAME) / sizeof(BASE), BASE, oni_detail::storage_##NAME> {                      \
       template<oni::i32 n>                                                                                             \
       inline static constexpr                                                                                          \
       NAME GET(const oni::c8 (&name)[n])                                                                               \
       { return {_get(name)};}                                                                                          \
       inline static constexpr const NAME* begin() { return static_cast<const NAME*>(_begin()); }                       \
       inline static constexpr const NAME* end() { return static_cast<const NAME*>(_end()); }                           \
};

#define ONI_ENUM_DEF(NAME, ...) ONI_ENUM_DEF_WITH_BASE(NAME, oni::Enum, __VA_ARGS__ )


namespace oni {
    struct Enum {
        oni::i32 id{0};
        oni::HashedString name{};

        using type = decltype(id);

        inline constexpr bool
        operator==(const Enum &other) const {
            if (other.name == name) {
                assert(other.id == id);
                return true;
            }
            return false;
        }

        inline constexpr bool
        operator!=(const Enum &other) const {
            if (other.name == name) {
                assert(other.id == id);
                return false;
            }
            return true;
        }


        inline std::string
        debugData() const {
            std::ostringstream buffer;
            buffer << "Name: ";
            buffer << name.str;
            buffer << ", id: ";
            buffer << id;
            buffer << ", hash: ";
            buffer << name.hash.value;
            return buffer.str();
        }
    };

    namespace oni_detail {
        template<oni::i32 N, class BASE, auto v>
        struct EnumBase : public BASE {
            inline bool
            valid() {
                return BASE::name != INVALID.name;
            }

            inline constexpr bool
            operator==(const BASE &other) const {
                if (other.name == BASE::name) {
                    assert(other.id == BASE::id);
                    return true;
                }
                return false;
            }

            inline constexpr bool
            operator!=(const BASE &other) const {
                if (other.name == BASE::name) {
                    assert(other.id == BASE::id);
                    return false;
                }
                return true;
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


            // TODO: I don't like this, this assumes the IDs start from 0 and index is equal to enum.id all the way.
            // I could support that with a look-up table though.
            inline static BASE
            at(oni::i32 i) {
                if (i >= 0 && i < N) {
                    return storage[i];
                }
                assert(false);
                return INVALID;
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
                static Out result[N];
                if (init) {
                    for (oni::i32 i = 0; i < N; ++i) {
                        result[i] = adaptor(storage[i]);
                    }
                    init = false;
                }
                return result;
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
                assert(false);
            }

        protected:
            template<oni::i32 n>
            static inline constexpr BASE
            _get(const oni::c8 (&name_)[n]) {
                return _find(oni::HashedString::makeHashFromLiteral(name_), 0, storage);
            }

            inline static constexpr auto
            _begin() {
                return storage;
            }

            inline static constexpr auto
            _end() {
                return storage + N;
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
            return lhs.name.hash.value == rhs.name.hash.value;
        }
    };
}
