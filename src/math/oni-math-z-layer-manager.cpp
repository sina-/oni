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
        mEntityZLayers[EntityType::BACKGROUND] = getNextZAtLayer(ZLayerDef::LAYER_0);
        mEntityZLayers[EntityType::ROAD] = getNextZAtLayer(ZLayerDef::LAYER_0);
        mEntityZLayers[EntityType::WALL] = getNextZAtLayer(ZLayerDef::LAYER_0);

        mEntityZLayers[EntityType::CANVAS] = getNextZAtLayer(ZLayerDef::LAYER_1);

        mEntityZLayers[EntityType::RACE_CAR] = getNextZAtLayer(ZLayerDef::LAYER_2);
        mEntityZLayers[EntityType::VEHICLE] = mEntityZLayers[EntityType::RACE_CAR];

        mEntityZLayers[EntityType::VEHICLE_GUN] = getNextZAtLayer(ZLayerDef::LAYER_2);

        mEntityZLayers[EntityType::SMOKE_CLOUD] = getNextZAtLayer(ZLayerDef::LAYER_4);
        mEntityZLayers[EntityType::TRAIL_PARTICLE] = getNextZAtLayer(ZLayerDef::LAYER_4);
        mEntityZLayers[EntityType::SIMPLE_PARTICLE] = getNextZAtLayer(ZLayerDef::LAYER_4);

        mEntityZLayers[EntityType::UI] = getNextZAtLayer(ZLayerDef::LAYER_8);
    }

    r32
    ZLayerManager::getZForEntity(EntityType type) const {
        assert(mEntityZLayers.find(type) != mEntityZLayers.end());
        return mEntityZLayers.at(type);
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
