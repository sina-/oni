#pragma once

#include <cstddef>

namespace oni {
    typedef signed char i8;
    typedef unsigned char u8;
    typedef signed short int i16;
    typedef unsigned short int u16;
    typedef signed int i32;
    typedef unsigned int u32;
    typedef signed long int i64;
    typedef unsigned long int u64;

    typedef std::size_t size;
    //using size = decltype(sizeof(1));
    typedef float r32;
    typedef double r64;

    typedef u32 u16p;
    typedef u64 u32p;
    typedef u64 i32p;
    typedef u32 EntityID;
    typedef r32 duration; // Duration in fraction of seconds.
}
