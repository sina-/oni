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
#define ENUM_DEFINE(ENUM, ID, VALUE)            \
    static ENUM                                 \
    VALUE() {                                   \
        return {ID, HashedString(#VALUE)};      \
    }

    struct Enum {
        size idx{};
        HashedString string{};

        template<class Archive>
        void
        save(Archive &archive) const {
            // NOTE: idx is not serialized
            archive("name", string.data);
        }

        template<class Archive>
        void
        load(Archive &archive) {
            archive("name", string.data);
            string.hash = hashString(string.data);
        }

        bool
        operator==(const Enum &other) const {
            return string.hash == other.string.hash;
        }

        bool
        operator==(const HashedString &other) const {
            return string.hash == other.hash;
        }

        bool
        operator!=(const HashedString &other) const {
            return string.hash != other.hash;
        }

        const c8 *
        name() const {
            return string.data.c_str();
        }
    };

    struct EnumVal {
        int value;
        const char *name;
    };

    template<class T, size N>
    struct Enums {
        T values[N];

        template<class ...V>
        Enums(V...v) : values{v...} {}

        static const auto &
        getInvalidEnum() {
            static auto invalidEnum = T{0, HashedString("__INVALID__")};
            return invalidEnum;
        }

        const auto *
        array() const {
            static EnumVal result[N];
            for (size i = 0; i < count(); ++i) {
                result[i].value = values[i].idx;
                result[i].name = values[i].name();
            }
            return &result;
        }

        size
        count() const {
            return sizeof(values) / sizeof(T);
        }

        // TODO: This should not be necessary. Each Enum already has idx but it is always 0 atm, once that is fixed
        // remove this.
        size
        getIndex(const T &value) {
            for (auto &&v: values) {
                if (v.string.hash == value.string.hash) {
                    return v.idx;
                }
            }
            assert(false);
            return 0;
        }

        const T &
        get(size id) const {
            if (id < values.size()) {
                return values[id];
            }

            assert(false);
            return getInvalidEnum();
        }

        const T &
        get(const c8 *name) const {
            auto hash = hashString(name);
            for (auto &&e: values) {
                if (e.string.hash == hash) {
                    return e;
                }
            }
            assert(false);
            return getInvalidEnum();
        }

        const T &
        get(const HashedString &name) const {
            for (auto &&e: values) {
                if (e == name) {
                    return e;
                }
            }
            assert(false);
            return getInvalidEnum();
        }

        bool
        valid(const T &v) const {
            return get(v.string).string.hash != getInvalidEnum().string.hash;
        }

        const T &
        operator()(const c8 *name) const {
            return get(name);
        }
    };
}