#pragma once

#include <vector>
#include <memory>

namespace oni {
    namespace components {
        class BufferStructure;
    }

    namespace common {
        typedef unsigned long oniSoundID;
        typedef double carSimDouble;

        typedef std::vector<std::unique_ptr<const components::BufferStructure>> BufferStructures;

        typedef unsigned long entityID;
    }
}
