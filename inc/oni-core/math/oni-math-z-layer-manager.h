#pragma once

#include <unordered_map>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-entity.h>
#include <oni-core/graphic/oni-graphic-camera.h>
#include <oni-core/math/oni-math-fwd.h>


namespace oni {
    class ZLayerManager {
    public:
        ZLayerManager();

        explicit ZLayerManager(const ZLayer &);

        r32
        getZForEntity(EntityType) const;

        r32
        getNextZAtLayer(ZLayerDef);

        ZLayer
        getZLayer() const;

    private:
        void
        constructEntityLayers();

    private:
        const r32 mMajorLayerDelta{0.1f};
        const r32 mMinorLayerDelta{0.001f};

        ZLayer mZLayer{};
        ZLayer mZLayerTop{};

        std::unordered_map<EntityType, r32> mEntityZLayers;
    };
}
