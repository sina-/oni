#include <oni-core/math/oni-math-z-layer-manager.h>

#include <cassert>


namespace oni {
    ZLayerManager::ZLayerManager() {
        mZLayer[ZLayer::GET("LAYER_0").id] = 0.f;
        mZLayer[ZLayer::GET("LAYER_1").id] = mZLayer[ZLayer::GET("LAYER_0").id] + mMajorLayerDelta;
        mZLayer[ZLayer::GET("LAYER_2").id] = mZLayer[ZLayer::GET("LAYER_1").id] + mMajorLayerDelta;
        mZLayer[ZLayer::GET("LAYER_3").id] = mZLayer[ZLayer::GET("LAYER_2").id] + mMajorLayerDelta;
        mZLayer[ZLayer::GET("LAYER_4").id] = mZLayer[ZLayer::GET("LAYER_3").id] + mMajorLayerDelta;
        mZLayer[ZLayer::GET("LAYER_5").id] = mZLayer[ZLayer::GET("LAYER_4").id] + mMajorLayerDelta;
        mZLayer[ZLayer::GET("LAYER_6").id] = mZLayer[ZLayer::GET("LAYER_5").id] + mMajorLayerDelta;
        mZLayer[ZLayer::GET("LAYER_7").id] = mZLayer[ZLayer::GET("LAYER_6").id] + mMajorLayerDelta;
        mZLayer[ZLayer::GET("LAYER_8").id] = mZLayer[ZLayer::GET("LAYER_7").id] + mMajorLayerDelta;
        mZLayer[ZLayer::GET("LAYER_9").id] = mZLayer[ZLayer::GET("LAYER_8").id] + mMajorLayerDelta;

        mZLayerTop = mZLayer;
    }

    r32
    ZLayerManager::getZAt(const ZLayer &layer) const {
        assert(layer.id >= ZLayer::GET("LAYER_0").id &&
               layer.id <= ZLayer::GET("LAYER_9").id);
        return mZLayer[layer.id];
    }

    r32
    ZLayerManager::getNextZAtLayer(const ZLayer &layer) {
        assert(layer.id >= ZLayer::GET("LAYER_0").id &&
               layer.id <= ZLayer::GET("LAYER_9").id);

        auto currentTop = mZLayerTop[layer.id];

        assert(currentTop + mMinorLayerDelta < mZLayer[layer.id] + mMajorLayerDelta);

        mZLayerTop[layer.id] = currentTop + mMinorLayerDelta;
        return currentTop;
    }

}
