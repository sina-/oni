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

        explicit ZLayerManager(const ZLayer &);

        void
        registerZ(const EntityName &name,
                  ZLayerDef def);

        void
        registerEqualZ(const EntityName &src,
                       const EntityName &dest);

        r32
        getZForEntity(EntityName) const;

        ZLayer
        getZLayer() const;

    private:
        r32
        getNextZAtLayer(ZLayerDef);

    private:
        const r32 mMajorLayerDelta{0.1f};
        const r32 mMinorLayerDelta{0.001f};

        ZLayer mZLayer{};
        ZLayer mZLayerTop{};

        std::unordered_map<Hash, r32> mEntityZLayers;
    };
}
