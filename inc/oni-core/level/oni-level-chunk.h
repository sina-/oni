#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/math/oni-math-vec3.h>

// TODO: Move to game

namespace oni {
    namespace level {
        struct ChunkIndex {
            i32 x{0};
            i32 y{0};
        };

        struct RoadTileIndex {
            // NOTE: This is relative to Chunk
            u16 x{0};
            u16 y{0};
        };

        struct TileIndex {
            i32 x{0};
            i32 y{0};
        };

        struct EdgeRoadTile {
            RoadTileIndex eastBoarder{};
            RoadTileIndex southBoarder{};
            RoadTileIndex westBoarder{};
            RoadTileIndex northBoarder{};
        };

        struct Chunk {
            u64 index{0};
            EdgeRoadTile edgeRoad{};
        };
    }
}