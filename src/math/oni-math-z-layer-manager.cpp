#include <oni-core/math/oni-math-z-layer-manager.h>

#include <cassert>


namespace oni {
    ZLayerManager::ZLayerManager() {
        mZLayer[ZLayer::GET("LAYER_0")] = 0.f;
        mZLayer[ZLayer::GET("LAYER_1")] = mZLayer[ZLayer::GET("LAYER_0")] + mMajorLayerDelta;
        mZLayer[ZLayer::GET("LAYER_2")] = mZLayer[ZLayer::GET("LAYER_1")] + mMajorLayerDelta;
        mZLayer[ZLayer::GET("LAYER_3")] = mZLayer[ZLayer::GET("LAYER_2")] + mMajorLayerDelta;
        mZLayer[ZLayer::GET("LAYER_4")] = mZLayer[ZLayer::GET("LAYER_3")] + mMajorLayerDelta;
        mZLayer[ZLayer::GET("LAYER_5")] = mZLayer[ZLayer::GET("LAYER_4")] + mMajorLayerDelta;
        mZLayer[ZLayer::GET("LAYER_6")] = mZLayer[ZLayer::GET("LAYER_5")] + mMajorLayerDelta;
        mZLayer[ZLayer::GET("LAYER_7")] = mZLayer[ZLayer::GET("LAYER_6")] + mMajorLayerDelta;
        mZLayer[ZLayer::GET("LAYER_8")] = mZLayer[ZLayer::GET("LAYER_7")] + mMajorLayerDelta;
        mZLayer[ZLayer::GET("LAYER_9")] = mZLayer[ZLayer::GET("LAYER_8")] + mMajorLayerDelta;

        mZLayerTop = mZLayer;
    }

    r32
    ZLayerManager::getZAt(const ZLayer &layer) const {
        assert(layer >= ZLayer::GET("LAYER_0") &&
               layer <= ZLayer::GET("LAYER_9"));
        return mZLayer[layer];
    }

    r32
    ZLayerManager::getNextZAtLayer(const ZLayer &layer) {
        assert(layer >= ZLayer::GET("LAYER_0") &&
               layer <= ZLayer::GET("LAYER_9"));

        auto currentTop = mZLayerTop[layer];

        assert(currentTop + mMinorLayerDelta < mZLayer[layer] + mMajorLayerDelta);

        mZLayerTop[layer] = currentTop + mMinorLayerDelta;
        return currentTop;
    }

}
