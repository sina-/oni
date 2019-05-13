#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/math/oni-math-vec3.h>

namespace oni {
    namespace level {
        struct ChunkIndex {
            common::i64 x{0};
            common::i64 y{0};
        };

        struct RoadTileIndex {
            // NOTE: This is relative to Chunk
            common::u16 x{0};
            common::u16 y{0};
        };

        struct TileIndex {
            common::i64 x{0};
            common::i64 y{0};
        };

        struct EdgeRoadTile {
            RoadTileIndex eastBoarder{};
            RoadTileIndex southBoarder{};
            RoadTileIndex westBoarder{};
            RoadTileIndex northBoarder{};
        };

        struct Chunk {
            common::u64 index{0};
            EdgeRoadTile edgeRoad{};
        };
    }
}