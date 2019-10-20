#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/util/oni-util-hash.h>

namespace oni {
    struct Enum {
        size idx{};
        HashedString string{};

        template<class Archive>
        void
        save(Archive &archive) const {
            // NOTE: idx is not serialized
            const auto &name = string.data;
            archive(name);
        }

        template<class Archive>
        void
        load(Archive &archive) {
            auto &name = string.data;
            archive(name);
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

        auto
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

        const auto &
        get(size id) const {
            if (id < values.size()) {
                return values[id];
            }

            assert(false);
            return getInvalidEnum();
        }

        const auto &
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

        const auto &
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

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive(values);
        }
    };
}