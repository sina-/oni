#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/util/oni-util-hash.h>
#include <oni-core/math/oni-math-function.h>

/* Criteria for a better enum type:
 * 1) minimal macro and template use
 * 2) simple io from and to string
 * 3) possibility to iterate over
 * 4) header only
 * 5) storage should require 32-bit
 * 6) ::ENUM_NAME or something quite similar should be available on the enum
 * 7) easy to use in if statements
 */


namespace oni {
    namespace { // NOTE: Anonymous namespace so that I can define static members in the header without linker errors
        template<class T>
        struct Enum_Base {
            int id{};

            bool
            operator==(int other) const {
                return other == id;
            }

            template<class Archive>
            void
            save(Archive &archive) {
                archive(map[id].string.data);
            }

            template<class Archive>
            void
            load(Archive &archive) {
                auto string = std::string();
                archive(string);
                auto hash = hashString(string);
                for (size i = 0; i < count; ++i) {
                    if (hash == map[i].hash) {
                        id = i;
                        return;
                    }
                }
                assert(false);
                id = 0;
            }

            static const char *
            name(int id) {
                if (id < count) {
                    return map[id].data.c_str();
                } else {
                    assert(false);
                    return "";
                }
            }

            template<class Tuple>
            static const auto *
            array() {
                static Tuple result[count];
                for (size i = 0; i < count; ++i) {
                    result[i].Value = i;
                    result[i].Label = name(i);
                }
                return &result;
            }

        protected:
            static HashedString *map;
            static int count;
        };

        template<class T>
        HashedString *Enum_Base<T>::map;

        template<class T>
        int Enum_Base<T>::count;
    }

    /// ===============================================================================================

#if defined(__clang__) || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER)
#define ENUM_TO_STRING_SUPPORTED 1
#else
#define ENUM_TO_STRING_SUPPORTED 0
#endif

    constexpr std::string_view
    removeSuffixFrom(std::string_view name,
                     size i) noexcept {
        name.remove_suffix(i);
        return name;
    }

    constexpr std::string_view
    removePrefixFrom(std::string_view name,
                     size i) noexcept {
        name.remove_prefix(i);
        return name;
    }

    template<typename E>
    using is_scoped_enum = std::integral_constant<bool, std::is_enum_v<E> && !std::is_convertible_v<E, int>>;

    template<auto v>
    constexpr std::string_view
    enumToStr() {
        static_assert(std::is_enum_v<decltype(v)>, "only enum should be used");
#if defined(ENUM_TO_STRING_SUPPORTED) && ENUM_TO_STRING_SUPPORTED
#  if defined(__clang__) || defined(__GNUC__)
        constexpr auto name = std::string_view(__PRETTY_FUNCTION__);
#  elif defined(_MSC_VER)
        // TODO: Test this
    constexpr auto name = pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#  endif
        if constexpr (is_scoped_enum<decltype(v)>::value) {
            auto clean = removePrefixFrom(name, 40);
            return removeSuffixFrom(clean, 1);// Remove trailing ] in the name
        } else {
            auto clean = removePrefixFrom(name, 34);
            return removeSuffixFrom(clean, 1);// Remove trailing ] in the name
        }
#else
        static_assert(std::false_type::value, "you need __PRETTY_FUNCTION__ or similar support to print enum strings");
    return std::string_view{};
#endif
    }

}