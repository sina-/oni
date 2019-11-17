#pragma once

namespace oni {
    typedef char c8;
    typedef signed char i8;
    typedef unsigned char u8;
    typedef signed short int i16;
    typedef unsigned short int u16;
    typedef signed int i32;
    typedef unsigned int u32;
    typedef signed long int i64;
    typedef unsigned long int u64;

    using size = decltype(sizeof(0));
    typedef float r32;
    typedef double r64;

    typedef u16 u8p;
    typedef u32 u16p;
    typedef u64 u32p;
    typedef u64 i32p;
    typedef u32 EntityID;
    typedef r32 duration32; // Duration in fraction of seconds
    typedef r64 duration64;

    typedef size AnimationID;
    typedef u8 FrameID;
}
