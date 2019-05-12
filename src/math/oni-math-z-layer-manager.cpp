#include <oni-core/math/oni-math-z-layer-manager.h>

#include <assert.h>

namespace oni {
    namespace math {
        ZLayerManager::ZLayerManager() {
            using namespace graphic;

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

        ZLayerManager::ZLayerManager(const graphic::ZLayer &zLayer) : mZLayer(zLayer) {
            constructEntityLayers();
        }

        void
        ZLayerManager::constructEntityLayers() {
            using namespace entities;
            using namespace graphic;
            mEntityZLayers[EntityType::BACKGROUND] = getNextZAtLayer(ZLayerDef::LAYER_0);
            mEntityZLayers[EntityType::ROAD] = getNextZAtLayer(ZLayerDef::LAYER_0);
            mEntityZLayers[EntityType::WALL] = getNextZAtLayer(ZLayerDef::LAYER_0);

            mEntityZLayers[EntityType::CANVAS] = getNextZAtLayer(ZLayerDef::LAYER_1);

            mEntityZLayers[EntityType::RACE_CAR] = getNextZAtLayer(ZLayerDef::LAYER_2);
            mEntityZLayers[EntityType::VEHICLE] = mEntityZLayers[EntityType::RACE_CAR];

            mEntityZLayers[EntityType::VEHICLE_GUN] = getNextZAtLayer(ZLayerDef::LAYER_2);

            mEntityZLayers[EntityType::UI] = getNextZAtLayer(ZLayerDef::LAYER_8);
        }

        common::real32
        ZLayerManager::getZForEntity(entities::EntityType type) const {
            assert(mEntityZLayers.find(type) != mEntityZLayers.end());
            return mEntityZLayers.at(type);
        }

        common::real32
        ZLayerManager::getNextZAtLayer(graphic::ZLayerDef layer) {
            assert(layer >= graphic::ZLayerDef::LAYER_0 &&
                   layer <= graphic::ZLayerDef::LAYER_9);

            auto currentTop = mZLayerTop[layer];

            assert(currentTop + mMinorLayerDelta < mZLayer[layer] + mMajorLayerDelta);

            mZLayerTop[layer] = currentTop + mMinorLayerDelta;
            return currentTop;
        }

        graphic::ZLayer
        ZLayerManager::getZLayer() const {
            return mZLayer;
        }
    }
}