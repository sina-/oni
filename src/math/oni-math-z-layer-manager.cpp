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
    }

    void
    ZLayerManager::setZ(const EntityName &name,
                        ZLayerDef def) {
        auto layer = _getNextZAtLayer(def);
        auto result = mEntityZLayers.emplace(name.hash, layer);
        if (!result.second) {
            assert(false);
        }
    }

    void
    ZLayerManager::setEqualZ(const EntityName &src,
                             const EntityName &dest) {
        auto layer = mEntityZLayers.find(src.hash);
        if (layer != mEntityZLayers.end()) {
            auto result = mEntityZLayers.emplace(dest.hash, layer->second);
            if (!result.second) {
                assert(false);
            }
        }
    }

    r32
    ZLayerManager::getZForEntity(const EntityName &name) const {
        auto layer = mEntityZLayers.find(name.hash);
        if (layer != mEntityZLayers.end()) {
            return layer->second;
        }
        assert(false);
        return 0.f;
    }

    r32
    ZLayerManager::_getNextZAtLayer(ZLayerDef layer) {
        assert(layer >= ZLayerDef::LAYER_0 &&
               layer <= ZLayerDef::LAYER_9);

        auto currentTop = mZLayerTop[layer];

        assert(currentTop + mMinorLayerDelta < mZLayer[layer] + mMajorLayerDelta);

        mZLayerTop[layer] = currentTop + mMinorLayerDelta;
        return currentTop;
    }

}
