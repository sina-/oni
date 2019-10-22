#pragma once

#include <iostream>
#include <cassert>
#include <cstdio>

constexpr int MAX_ENUM_SIZE = 512;

using size = size_t;
using c8 = char;
using Hash = size;

template<class T>
constexpr std::underlying_type_t<T>
enumCast(T value) noexcept {
    return static_cast<std::underlying_type_t<T>>(value);
}

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

namespace test_a {
    template<class T>
    struct BetterEnum {
        T value{};

        explicit BetterEnum(T value) : value(value) {
            assert(enumCast(T::LAST) < MAX_ENUM_SIZE);
        }

        BetterEnum
        operator()(T v) {
            return BetterEnum(v);
        }

        std::underlying_type_t<T>
        id() {
            return enumCast(value);
        }

        std::underlying_type_t<T>
        constexpr
        count() {
            return enumCast(T::LAST);
        }

        template<class Archive>
        void
        save(Archive &archive) {
            archive(map[value].string.data);
        }

        template<class Archive>
        void
        load(Archive &archive) {
            auto string = std::string();
            archive(string);
            auto hash = hashString(string);
            for (size i = 0; i < enumCast(T::LAST); ++i) {
                if (hash == map[i].hash) {
                    value = i;
                    return;
                }
            }
            assert(false);
            value = 0;
        }

        static Enum map[enumCast(T::LAST)];
    };

    union MF {
    public:
        enum _ {
            A,
            B,
            C,

            LAST
        };

        BetterEnum<_> value{_::A};
    };

    union T {
    public:
        enum {
            A
        };
    };


    static void
    test() {
        auto a = MF{};
        for (int i = 0; i < a.value.count(); ++i) {

        }
        if (MF::A == a.value.id()) {

        }
    }
}
namespace test_b {
#define ENUM_DEFINE(ENUM, ID, VALUE)            \
    static ENUM                                 \
    VALUE() {                                   \
        return {ID, HashedString(#VALUE)};      \
    }

    // TODO: How do I deal with iteration in this setup?
    struct Material_Finish_Enum_INH : public Enum {
        ENUM_DEFINE(Material_Finish_Enum_INH, 0, SOLID)

        ENUM_DEFINE(Material_Finish_Enum_INH, 1, TRANSLUCENT)

        ENUM_DEFINE(Material_Finish_Enum_INH, 2, SHINNY)

        template<class Archive>
        void
        load(Archive &archive) {
            auto name = HashedString();
            archive(name);

            if (SOLID() == name) {
                *this = SOLID();
            } else if (TRANSLUCENT() == name) {
                *this = TRANSLUCENT();
            } else if (SHINNY() == name) {
                *this = TRANSLUCENT();
            } else {
                assert(false);
                *this = SOLID();
            }
        }
    };

    struct Material_Finish_Enum_BARE {
        Enum value;

        ENUM_DEFINE(Material_Finish_Enum_BARE, 0, SOLID)

        ENUM_DEFINE(Material_Finish_Enum_BARE, 1, TRANSLUCENT)

        ENUM_DEFINE(Material_Finish_Enum_BARE, 2, SHINNY)

        template<class Archive>
        void
        save(Archive &archive) {
            archive(value.string);
        }

        template<class Archive>
        void
        load(Archive &archive) {
            auto name = HashedString();
            archive(name);

            if (SOLID().value == name) {
                *this = SOLID();
            } else if (TRANSLUCENT().value == name) {
                *this = TRANSLUCENT();
            } else if (SHINNY().value == name) {
                *this = TRANSLUCENT();
            } else {
                assert(false);
                *this = SOLID();
            }
        }
    };
}

namespace test_c {
    enum class Material_Finish_Enum {
        // NOTE: The order of the enums defines the order in which they are rendered!
        // NOTE: For translucent and shinny entities since depth writes are disabled
        // if an entity that is translucent but has higher z is rendered then a shinny
        // entity with lower z is rendered it will still be drawn over the higher z
        // entity!

        SOLID,
        TRANSLUCENT,
        SHINNY,

        LAST
    };

    // TODO: DO I really want to complicate traditional enum usage for this much complexity?
    // What am I gaining by this? Just a mapping of int to string? I could also do this you know:
    struct Material_Finish {
        Material_Finish_Enum value;

        template<class Archive>
        void
        save(Archive &archive) const {
            switch (value) {
                case Material_Finish_Enum::SOLID: {
                    archive("solid");
                    break;
                }
                case Material_Finish_Enum::TRANSLUCENT: {
                    archive("translucent");
                    break;
                }
                case Material_Finish_Enum::SHINNY : {
                    archive("shinny");
                    break;
                }
                default : {
                    archive("solid");
                    assert(false);
                }
            }
        }

        template<class Archive>
        void
        load(Archive &archive) {
            auto string = HashedString();
            archive(string.data);
            string.hash = hashString(string.data);

            const auto solid = HashedString("solid");
            const auto translucent = HashedString("translucent");
            const auto shinny = HashedString("shinny");
            if (string == solid) {
                value = Material_Finish_Enum::SOLID;
            } else if (string == translucent) {
                value = Material_Finish_Enum::TRANSLUCENT;
            } else if (string == shinny) {
                value = Material_Finish_Enum::SHINNY;
            } else {
                assert(false);
                value = Material_Finish_Enum::SOLID;
            }
        }
    };
}

namespace test_d {
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

    enum class Material_Finish_Enum {
        // NOTE: The order of the enums defines the order in which they are rendered!
        // NOTE: For translucent and shinny entities since depth writes are disabled
        // if an entity that is translucent but has higher z is rendered then a shinny
        // entity with lower z is rendered it will still be drawn over the higher z
        // entity!

        SOLID,
        TRANSLUCENT,
        SHINNY,

        LAST
    };

    struct __Material_Finish : public Enum {
    };

    static auto _Material_Finish_Enum = Enums<__Material_Finish, 3>{
            Enum{0, HashedString("solid")},
            Enum{1, HashedString("translucent")},
            Enum{2, HashedString("shinny")},
    };

}

namespace test_x {
    namespace {
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

//            T
//            operator=(int other) {
//                return {other};
//            }

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

        protected:
            static HashedString *map;
            static int count;
        };

        template<class T>
        HashedString *Enum_Base<T>::map;

        template<class T>
        int Enum_Base<T>::count;
    }

    struct Enum_P : public Enum_Base<Enum_P> {
        Enum_P() : Enum_Base() {
            static bool init = true;
            if (init) {
                count = enumCast(member::LAST);
                map = new HashedString[count];
                map[0] = HashedString("A");
                map[1] = HashedString("B");
                map[2] = HashedString("C");
                init = false;
            }
        }

        Enum_P(int id) : Enum_Base{id} {}

        enum member {
            A,
            B,
            C,

            LAST
        };

        Enum_P
        operator=(member other) {
            return {other};
        }
    };

//    template<>
//    int Enum_Base<Enum_P>::count = Enum_P::LAST;
//    template<>
//    HashedString *Enum_Base<Enum_P>::map = new HashedString[Enum_P::LAST];
//    template<>
//    HashedString* Enum_Base<Enum_P>::map = HashedString("A");

//    template<>
//    Enum Enum_Base<3>::map[] = {
//            Enum{0, HashedString("A")},
//            Enum{1, HashedString("B")},
//            Enum{2, HashedString("C")},
//    };

    static void
    test() {
        auto a = Enum_P{};
        if (a.id == Enum_P::A) {
            printf("YES\n");
        }
        if (a == Enum_P::A) {
            printf("YES2\n");
        }

        for (int i = 0; i < Enum_P::LAST; ++i) {
            printf("%s\n", Enum_P::name(i));
        }
        Enum_P b = Enum_P::A;
        if (b == Enum_P::A) {
            printf("YES3\n");
        }
        if (b == Enum_P::B) {
            printf("WTF\n");
        }

        struct Material_Definition {
            Enum_P finish{Enum_P::B};
        };

        /*
         * 1) minimal macro and template use - CHECK
         * 2) simple io from and to string - CHECK
         * 3) possibility to iterate over - CHECK
         * 4) header only - CHECK
         * 5) storage should require 32-bit - CHECK
         * 6) ::ENUM_NAME or something quite similar should be available on the enum - CHECK
         * 7) easy to do if on - CHECK
         */
    }
}
