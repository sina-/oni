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


#define CONCAT(A, B) A ## B

#define ONI_ENUM_DEF(NAME, ...)                                                                                                         \
    namespace detail{ namespace {inline static const constexpr oni::detail::Enum storage_##NAME [] = { __VA_ARGS__};}} \
    struct NAME : public oni::detail::EnumBase<                                                                        \
            sizeof(detail::storage_##NAME) / sizeof(oni::detail::Enum), detail::storage_##NAME> {                      \
       inline constexpr NAME() : EnumBase(0, detail::storage_##NAME[0].name) {}                                        \
       inline constexpr NAME(oni::i32 id_, const oni::HashedString &name_) : EnumBase(id_, name_) {}                   \
       inline constexpr NAME(const oni::detail::Enum& value_) : EnumBase(value_.id, value_.name) {}                    \
    };                                                                                                                 \
    template<oni::i32 n>                                                                                               \
    inline static constexpr NAME                                                                                       \
    CONCAT(NAME, _GET)(const oni::c8 (&name_)[n]) { return NAME::_convert<NAME>(NAME::_get(name_)); }

namespace oni {
    namespace detail {
        struct Enum {
            oni::i32 id;
            oni::HashedString name;
        };

        template<oni::i32 N, auto v>
        struct EnumBase {
            oni::i32 id;
            oni::HashedString name;

            inline constexpr
            EnumBase(oni::i32 id_,
                     oni::HashedString name_) noexcept : id(id_), name(name_) {}

            inline static constexpr auto
            begin() {
                return storage;
            }

            inline static constexpr auto
            end() {
                return storage + N;
            }

            inline constexpr bool
            operator==(const EnumBase &other) const {
                return other.id == id && other.name == name;
            }

            inline constexpr bool
            operator!=(const EnumBase &other) const {
                return other.id != id || other.name != name;
            }

            inline static constexpr oni::i32
            size() {
                return N;
            }

            template<class Archive>
            void
            save(Archive &archive) const {
                archive("name", name.str);
            }

            template<class Archive>
            void
            load(Archive &archive) {
                std::string buffer{};
                archive("name", buffer);
                name = oni::HashedString::view(buffer);
                // NOTE: After _init() hash will point to static storage so buffer is safe to go out of scope
                _runtimeInit(name.hash);
            }

            template<class Out, class Child, class Adaptor>
            static inline Out *
            array(Adaptor adaptor) {
                static auto result = std::array<Out, N>();
                for (oni::i32 i = 0; i < N; ++i) {
                    result[i] = adaptor(storage[i]);
                }
                return result.data();
            }

            template<oni::i32 n>
            static inline constexpr Enum
            _get(const oni::c8 (&name_)[n]) {
                return _find(oni::HashedString::get(name_), 0, storage);
            }

            template<class T>
            inline static constexpr T
            _convert(const Enum &value) {
                return T(value.id, value.name);
            }

        private:
            // TODO: How can I make this better, the diagnosis error is misleading
            inline static constexpr Enum
            _notFound() {
                throw std::logic_error("Invalid value specified for the enum");
                return INVALID;
            }

            inline static constexpr Enum
            _find(const oni::Hash hash,
                  oni::i32 i,
                  const Enum *candidate) {
                return (candidate->name.hash == hash) ? *candidate :
                       ((i == N) ? _notFound() : _find(hash, i + 1, candidate + 1));
            }

            void
            _runtimeInit(const oni::Hash hash) {
                for (oni::i32 i = 0; i < N; ++i) {
                    if (storage[i].name.hash == hash) {
                        name = storage[i].name;
                        id = storage[i].id;
                        return;
                    }
                }
                assert(false);
                id = 0;
                name = INVALID.name;
            }

        private:
            static inline const constexpr Enum *storage = v;
            static inline const constexpr Enum INVALID = {0, oni::HashedString("__INVALID__")};
        };
    }
}
