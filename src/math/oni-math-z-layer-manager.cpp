#include <oni-core/math/oni-math-z-layer-manager.h>

#include <cassert>


namespace oni {
    ZLayerManager::ZLayerManager() {
        mZLayer[ZLayerDef::LAYER_0] = 0.f;
        mZLayer[ZLayerDef::LAYER_1] = mZLayer[ZLayerDef::LAYER_0] + mMajorLayerDelta;
        mZLayer[ZLayerDef::LAYER_2] = mZLayer[ZLayerDef::LAYER_1] + mMajorLayerDelta;
        mZLayer[ZLayerDef::LAYER_3] = mZLayer[ZLayerDef::LAYER_2] + mMajorLayerDelta;
        mZLayer[ZLayerDef::LAYER_4] = mZLayer[ZLayerDef::LAYER_3] + mMajorLayerDelta;
        mZLayer[ZLayerDef::LAYER_5] = mZLayer[ZLayerDef::LAYER_4] + mMajorLayerDelta;
        mZLayer[ZLayerDef::LAYER_6] = mZLayer[ZLayerDef::LAYER_5] + mMajorLayerDelta;
        mZLayer[ZLayerDef::LAYER_7] = mZLayer[ZLayerDef::LAYER_6] + mMajorLayerDelta;
        mZLayer[ZLayerDef::LAYER_8] = mZLayer[ZLayerDef::LAYER_7] + mMajorLayerDelta;
        mZLayer[ZLayerDef::LAYER_9] = mZLayer[ZLayerDef::LAYER_8] + mMajorLayerDelta;

        mZLayerTop = mZLayer;

        constructEntityLayers();
    }

    ZLayerManager::ZLayerManager(const ZLayer &zLayer) : mZLayer(zLayer) {
        constructEntityLayers();
    }

    void
    ZLayerManager::constructEntityLayers() {
    }

    void
    ZLayerManager::setZForEntity(const EntityType &t,
                                 ZLayerDef def) {
        auto layer = getNextZAtLayer(def);
        mEntityZLayers.emplace(t.value, layer);
    }

    void
    ZLayerManager::setZForEntityEqual(const EntityType &src,
                                      const EntityType &dest) {
        assert(mEntityZLayers.find(src.value) != mEntityZLayers.end());
        mEntityZLayers[dest.value] = mEntityZLayers[src.value];
    }

    r32
    ZLayerManager::getZForEntity(EntityType type) const {
        assert(mEntityZLayers.find(type.value) != mEntityZLayers.end());
        return mEntityZLayers.at(type.value);
    }

    r32
    ZLayerManager::getNextZAtLayer(ZLayerDef layer) {
        assert(layer >= ZLayerDef::LAYER_0 &&
               layer <= ZLayerDef::LAYER_9);

        auto currentTop = mZLayerTop[layer];

        assert(currentTop + mMinorLayerDelta < mZLayer[layer] + mMajorLayerDelta);

        mZLayerTop[layer] = currentTop + mMinorLayerDelta;
        return currentTop;
    }

    ZLayer
    ZLayerManager::getZLayer() const {
        return mZLayer;
    }
}
