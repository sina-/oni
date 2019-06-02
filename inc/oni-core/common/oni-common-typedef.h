#pragma once

#include <string>

namespace oni {
    namespace common {
        typedef std::int64_t i64;
        typedef std::int32_t i32;
        typedef std::int16_t i16;
        typedef std::int8_t i8;

        typedef std::uint64_t u64;
        typedef std::uint32_t u32;
        typedef std::uint16_t u16;
        typedef std::uint8_t u8;
        typedef std::size_t size;

        typedef float r32;
        typedef double r64;

        typedef double CarSimDouble;

        typedef u64 i64p;

        typedef u32 u16p;

        typedef u64 u32p;

        typedef u32 EntityID;

        typedef std::string PeerID;

        typedef r32 duration; // Duration in fraction of seconds.
    }
}
