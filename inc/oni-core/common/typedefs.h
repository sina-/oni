#pragma once

#include <vector>
#include <memory>

namespace oni {
    namespace components {
        class BufferStructure;
    }

    namespace common {
        typedef std::int64_t int64;
        typedef std::int32_t int32;
        typedef std::int16_t int16;
        typedef std::int8_t int8;

        typedef std::uint64_t uint64;
        typedef std::uint32_t uint32;
        typedef std::uint16_t uint16;
        typedef std::uint8_t uint8;

        typedef float real32;
        typedef double real64;

        typedef double carSimDouble;
        typedef uint32 oniSoundID;

        typedef uint64 packedInt32;

        typedef uint32 EntityID;

        using BufferStructures = typename std::vector<std::unique_ptr<const components::BufferStructure>>;
    }
}
