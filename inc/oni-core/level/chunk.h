#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/math/vec3.h>

namespace oni {
    namespace level {
        struct ChunkIndex {
            common::int64 x{0};
            common::int64 y{0};
        };

        struct RoadTileIndex {
            // NOTE: This is relative to Chunk
            common::uint16 x{0};
            common::uint16 y{0};
        };

        struct TileIndex {
            common::int64 x{0};
            common::int64 y{0};
        };

        struct EdgeRoadTile {
            RoadTileIndex eastBoarder{};
            RoadTileIndex southBoarder{};
            RoadTileIndex westBoarder{};
            RoadTileIndex northBoarder{};
        };

        struct Chunk {
            math::vec3 position{0.f, 0.f, 0.f};
            common::uint64 index{0};
            EdgeRoadTile edgeRoad{};
        };
    }
}