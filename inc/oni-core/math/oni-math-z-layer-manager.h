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

        void
        setZForEntity(const EntityType &,
                      ZLayerDef);

        void
        setZForEntityEqual(const EntityType &src,
                           const EntityType &dest);

        r32
        getZForEntity(EntityType) const;

        ZLayer
        getZLayer() const;

    private:
        r32
        getNextZAtLayer(ZLayerDef);

        void
        constructEntityLayers();

    private:
        const r32 mMajorLayerDelta{0.1f};
        const r32 mMinorLayerDelta{0.001f};

        ZLayer mZLayer{};
        ZLayer mZLayerTop{};

        std::unordered_map<decltype(EntityType::value), r32> mEntityZLayers;
    };
}
