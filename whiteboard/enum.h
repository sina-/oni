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
    struct Material_Finish_INH : public Enum {
        ENUM_DEFINE(Material_Finish_INH, 0, SOLID)

        ENUM_DEFINE(Material_Finish_INH, 1, TRANSLUCENT)

        ENUM_DEFINE(Material_Finish_INH, 2, SHINNY)

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

    struct Material_Finish_BARE {
        Enum value;

        ENUM_DEFINE(Material_Finish_BARE, 0, SOLID)

        ENUM_DEFINE(Material_Finish_BARE, 1, TRANSLUCENT)

        ENUM_DEFINE(Material_Finish_BARE, 2, SHINNY)

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
    enum class Material_Finish {
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
        Material_Finish value;

        template<class Archive>
        void
        save(Archive &archive) const {
            switch (value) {
                case Material_Finish::SOLID: {
                    archive("solid");
                    break;
                }
                case Material_Finish::TRANSLUCENT: {
                    archive("translucent");
                    break;
                }
                case Material_Finish::SHINNY : {
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
                value = Material_Finish::SOLID;
            } else if (string == translucent) {
                value = Material_Finish::TRANSLUCENT;
            } else if (string == shinny) {
                value = Material_Finish::SHINNY;
            } else {
                assert(false);
                value = Material_Finish::SOLID;
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

    enum class Material_Finish {
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

    static auto _Material_Finish = Enums<__Material_Finish, 3>{
            Enum{0, HashedString("solid")},
            Enum{1, HashedString("translucent")},
            Enum{2, HashedString("shinny")},
    };

}

namespace test_e {
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

namespace test_f {
    using size = size_t;
    using Hash = size;
    using c8 = char;

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
            auto clean = removePrefixFrom(name, 41 + 5 + 5);
            return removeSuffixFrom(clean, 1);// Remove trailing ] in the name
        } else {
            auto clean = removePrefixFrom(name, 34 + 5 + 5);
            return removeSuffixFrom(clean, 1);// Remove trailing ] in the name
        }
#else
        static_assert(std::false_type::value, "you need __PRETTY_FUNCTION__ or similar support to print enum strings");
    return std::string_view{};
#endif
    }

    /// ===============================================================================================

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


        explicit HashedString(std::string_view value) noexcept: data(value), hash(hashString(data)) {}

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

    template<class ENUM, auto t>
    struct wrapper {
        void
        operator()(HashedString *map,
                   size i) const {
            constexpr auto e = static_cast<ENUM>(t);
            constexpr auto string = enumToStr<e>();
            map[i] = HashedString(string);
        }
    };


    template<template<class ENUM, int> class WRAPPER, class ENUM, std::size_t... I>
    void
    caller_impl(HashedString *map,
                std::index_sequence<I...>) {
        int t[] = {0, ((void) WRAPPER<ENUM, I>()(map, I), 1)...};
        (void) t;
    }

    template<template<class ENUM, int> class WRAPPER, class ENUM, std::size_t N, typename Indices = std::make_index_sequence<N>>
    void
    call_times(HashedString *map) {
        caller_impl<WRAPPER, ENUM>(map, Indices());
    }


    namespace { // NOTE: Anonymous namespace so that I can define static members in the header without linker errors
        template<class T, class ENUM>
        struct Enum_Base {
            int id{};

            explicit Enum_Base(int id_) : Enum_Base() { id = id_; }

            Enum_Base() {
                static bool init = true;
                if (init) {
                    count = enumCast(ENUM::_LAST);
                    map = new HashedString[count];

                    constexpr auto t = enumCast(ENUM::_LAST);
                    call_times<wrapper, ENUM, t>(map);
                    init = false;
                }
            }

            bool
            operator==(int other) const {
                return other == id;
            }

            ENUM
            operator=(int other) const {
                return static_cast<ENUM>(other);
            }

//            Enum_Base
//            operator=(ENUM other) const {
//                return Enum_Base(enumCast(other));
//            }

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

        template<class T, class ENUM>
        HashedString *Enum_Base<T, ENUM>::map;

        template<class T, class ENUM>
        int Enum_Base<T, ENUM>::count;
    }

    enum class _Enum {
        SOLID,
        TRANSLUCENT,
        SHINNY,

        _LAST
    };


    struct Enum : public Enum_Base<Enum, _Enum> {
        Enum() = default;

        explicit Enum(int id_) : Enum_Base() { id = id_; }

        enum _ {
            SOLID,
            TRANSLUCENT,
            SHINNY,

            _LAST
        };

        Enum(_ id_) : Enum_Base() { id = id_; }

        Enum
        operator=(const _ &other) const {
            return Enum{other};
        }
    };
    namespace {
        static const Enum _INIT_Enum{};
    }

#define DEFINE_ENUM(NAME, ...)                                                  \
enum class _INTERNAL_##NAME {                                                   \
        __VA_ARGS__                                                             \
        , _LAST                                                                 \
        };                                                                      \
        struct NAME : public Enum_Base<NAME, _INTERNAL_##NAME > {               \
        NAME() = default;                                                       \
        explicit NAME(int id_) : Enum_Base() {id = id_;}                        \
        enum _ {                                                                \
           __VA_ARGS__                                                          \
           , _LAST                                                              \
        };                                                                      \
        NAME(_ id_) : Enum_Base() { id = id_; }                                 \
        NAME                                                                    \
        operator=(const _&other) const {                                        \
           return NAME{other};                                                  \
        }                                                                       \
        };                                                                      \
        namespace {                                                             \
            static const NAME _INIT_##NAME{};                                   \
        }

    DEFINE_ENUM(WOW, RED, BLUE)


    inline void
    test() {
        WOW c = WOW::RED;
        auto a = Enum{0};
        if (a == Enum::SOLID) {
            printf("YES\n");
        }

        for (int i = 0; i < Enum::_LAST; ++i) {
            printf("%s\n", Enum::name(i));
        }
        Enum b = Enum::SOLID;
        if (b == Enum::SOLID) {
            printf("YES3\n");
        }
        if (b == Enum::SHINNY) {
            printf("WTF\n");
        }
    }

    /*
     * Issues:
     * Doesn't support numbered enums well: enum OOPS {FOO = 1, BAR = 1000};
     * Names of enum class are in again
     */
}

namespace test_g {
    using size = std::size_t;
    using c8 = char;
    using Hash = unsigned long long;
//
//template<size N>
//static constexpr Hash
//hashString(const c8 (&value)[N]) {
//    constexpr std::hash<const c8 (&)[N]> func;
//    return func(value);
//}

// TODO: I really need to do something about all these std::strings that are allocating memory in runtime
// the general concept of constexpr hashing should help with most of these allocations.
//static Hash
//hashString(const c8 *value) {
//    auto string = std::string(value);
//    return hashString(string);
//}
    static constexpr std::uint64_t offset = 14695981039346656037ull;
    static constexpr std::uint64_t prime = 1099511628211ull;

    struct HashedString {
        std::string_view data{};
        Hash hash{};

        constexpr HashedString() = default;

        constexpr HashedString(std::string_view data_,
                               Hash hash_) noexcept: data(data_), hash(hash_) {}

//    constexpr
//    HashedString &
//    operator=(const HashedString &other) noexcept = default;

        // TODO: The base of issue is that I want my HashedString to support constexpr chars and
        // runtime chars! Right now this only supports runtime chars.
//    explicit HashedString(const c8 *value) noexcept: data(value), hash(hashString(data)) {}
//
//    explicit HashedString(std::string &&value) noexcept: data(std::move(value)), hash(hashString(data)) {}

        template<std::size_t N>
        explicit constexpr
        HashedString(const c8 (&value)[N]) noexcept {
            data = value;
            hash = _getHash(value);
        }

        template<std::size_t N>
        static constexpr
        HashedString
        make(const c8 (&value)[N]) noexcept {
            return {value, _getHash(value)};
        }

        HashedString
        static constexpr
        make(std::string_view value) noexcept {
            return {value.data(), _getHash(value)};
        }

        constexpr
        const c8 *
        c_str() const {
            return data.data();
        }


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
        // TODO: Have to think about this, I can't just load a string as this class doesn't own any string data at the moment!
        // maybe after I have the string storage system?
//
//    template<class Archive>
//    void
//    save(Archive &archive) const {
//        // NOTE: hash is not serialized!
//        archive(data);
//    }
//
//    template<class Archive>
//    void
//    load(Archive &archive) {
//        archive(data);
//        hash = _helper<>(data.data());
//    }

    private:
        // Fowler–Noll–Vo hash
        static constexpr Hash
        _hash(Hash partial,
              const c8 current) {
            return (partial ^ current) * prime;
        }

        template<std::size_t N>
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

//    static constexpr Hash
//    helper(Hash partial,
//           const c8 *curr) noexcept {
//        return curr[0] == 0 ? partial : helper(_hash(partial, curr[0]), curr + 1);
//    }
    };

    using ENUM = HashedString;

    template<size n, class _child>
    struct EnumBase {
        size id{0};
        HashedString name{"__INVALID__"};

//            typedef EnumBase *iterator;
//            typedef const EnumBase *const_iterator;

        constexpr
        EnumBase(size id_,
                 HashedString name_) noexcept : id(id_), name(name_) {}

        static constexpr
        auto
        begin() {
            return map.begin() + 1;
        }

        static constexpr
        auto
        end() {
            return map.end();
        }

    protected:
        constexpr
        EnumBase() = default;

        constexpr explicit EnumBase(std::array<HashedString, n> &&map_) {
            map[0] = invalid;
            for (size i = 0; i < map_.size(); ++i) {
                map[i + 1] = map_[i];
            }
        }

        template<size N>
        constexpr
        EnumBase(const HashedString (&v)[N]) noexcept {
        }

        constexpr
        EnumBase(std::initializer_list<std::string_view> map_) {
            map[0] = invalid;
            auto *begin = map_.begin();
            for (size i = 0; i < map_.size(); ++i) {
                map[i + 1] = HashedString::make(*begin);
                ++begin;
            }
        }

        template<size N, class Child>
        static constexpr
        Child
        make(const c8 (&name)[N]) {
            return assignID<Child>(Child{0, HashedString::make(name)});
        }

//            constexpr
//            explicit EnumBase(std::initializer_list<HashedString> map_) {
//                map = map_;
//            }

//            template<class ... T>
//            void
//            init(T...m) {
//                map[]
//                { m... };
//            }

//            template<class HString>
//            void
//            init(const int i) {}
//
//            template<class HString, class ... Args>
//            constexpr void
//            init(const int i,
//                 const HString value,
//                 Args ... tail) {
//                map[i] = value;
//                init<HString>(i + 1, tail ...);
//            }

    private:
        template<class Child>
        static constexpr
        Child
        assignID(Child result) noexcept {
            result.id = _findID(result.name.hash);
            if (result.id == 0) {
                result.name = invalid;
            }
            return result;
        }

        static constexpr
        size
        _findID(const Hash value) {

            for (size i = 0; i < n; ++i) {
                // TODO: This is not constexpr :/ and I if constexpr is invalid
                if (map[i].hash == value) {
                    return i;
                }
            }
            assert(false);
            return 0;
        }

    private:
        static inline std::array<HashedString, n + 1> map;
        static inline HashedString invalid = HashedString("__INVALID__");
    };

    namespace detail {
        class INIT;
    }

    struct Test : public EnumBase<3, Test> {
    public:
        constexpr
        Test(size id_,
             HashedString name_) noexcept : EnumBase(id_, name_) {}

        template<size N>
        static constexpr Test
        make(const c8 (&name)[N]) {
            return EnumBase::make<N, Test>(name);
        }

    protected:
        friend detail::INIT;

        constexpr Test() : EnumBase({{"FOO"},
                                     {"BAR"},
                                     {"ZOO"}}) {}

    };

    namespace detail {
        class INIT {
            inline static Test _a = {};
        };
    }

#define ENUM_DEF(NAME, COUNT, ...)                                                                                  \
    namespace _detail {class INIT_##NAME ;}                                                                         \
    struct NAME : public EnumBase<COUNT, NAME> {                                                                    \
    public:                                                                                                         \
       constexpr NAME(size id_, HashedString name_) noexcept : EnumBase(id_, name_) {}                              \
                                                                                                                    \
       template<size N>                                                                                             \
       static constexpr NAME                                                                                        \
       make(const c8 (&name)[N]) { return EnumBase::make<N, NAME>(name); }                                          \
                                                                                                                    \
    protected:                                                                                                      \
    friend _detail::INIT_##NAME;                                                                                    \
    constexpr NAME() : EnumBase({ __VA_ARGS__ }) {}                                                                 \
    };                                                                                                              \
    namespace _detail { class INIT_##NAME { inline static NAME _STORAGE_FOR_##NAME = {}; };}                        \

    ENUM_DEF(What_A_Nice_Enum, 3, { "one" }, { "two" }, { "three" })

    void testtt {
        for (auto I = oni::Test::begin(); I != oni::Test::end(); ++I) {
            printf("%s\n", I->data.data());
        }

        auto a = oni::Test::make("BAR");
        printf("a id is %lu, data: %s\n", a.id, a.name.c_str());

        auto b = oni::Test::make("FOO");
        printf("b id is %lu, data: %s\n", b.id, b.name.c_str());

        auto c = test::What_A_Nice_Enum::make("two");
        printf("c id is %lu, data: %s\n", c.id, c.name.c_str());

        auto d = oni::Test::make("what");
        printf("d id is %lu, data: %s\n", d.id, d.name.c_str());
    };
// TODO:
// 1) initialization is bullshit with the friend class and shit
// 2) value() function can return invalid data, I can't do compile-time check at the moment :( But maybe I don't want
// to? either compiling will be slow or run-time! But I guess I won't compile everything but I wil run every init
// doing run-time so might be better off with compile time?
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
