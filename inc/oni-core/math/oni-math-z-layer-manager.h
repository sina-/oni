#pragma once

#include <unordered_map>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-structure.h>
#include <oni-core/graphic/oni-graphic-camera.h>
#include <oni-core/math/oni-math-fwd.h>


namespace oni {
    class ZLayerManager {
    public:
        ZLayerManager();

        r32
        getZAt(const ZLayer &) const;

        r32
        getNextZAtLayer(const ZLayer &);

    private:
        static inline constexpr r32 mMajorLayerDelta{0.1f};
        static inline constexpr r32 mMinorLayerDelta{0.001f};

        std::array<ZLayer::type, ZLayer::size()> mZLayer{};
        std::array<ZLayer::type, ZLayer::size()> mZLayerTop{};
    };
}
