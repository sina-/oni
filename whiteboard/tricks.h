#pragma once

namespace enum_macro {
#define DECLARE_ENUM_WITH_TYPE(E, T, ...)                                                                     \
    enum class E : T                                                                                          \
    {                                                                                                         \
        __VA_ARGS__                                                                                           \
    };
}

namespace constexpr_str_len {
    constexpr std::size_t
    constexpr_strlen(const char *str) { return str[0] == 0 ? 0 : constexpr_strlen(str + 1) + 1; }
}

namespace tail {
    template<typename ENUM, typename ... args>
    void
    initialize(const ENUM value,
               const char *name,
               args ... tail) {
        enum_values < ENUM >.emplace(value, name);
        initialize<ENUM>(tail ...);
    }
}