#pragma once

#include <unordered_map>

#include <oni-core/component/visual.h>
#include <oni-core/entities/entity.h>
#include <oni-core/graphic/camera.h>

namespace oni {
    namespace math {
        class ZLayerManager {
        public:
            ZLayerManager();

            explicit ZLayerManager(const graphic::ZLayer &);

            common::real32
            getZForEntity(entities::EntityType) const;

            common::real32
            getNextZAtLayer(graphic::ZLayerDef);

            graphic::ZLayer
            getZLayer() const;

        private:
            void
            constructEntityLayers();

        private:
            const common::real32 mMajorLayerDelta{0.1f};
            const common::real32 mMinorLayerDelta{0.001f};

            graphic::ZLayer mZLayer{};
            graphic::ZLayer mZLayerTop{};

            std::unordered_map<entities::EntityType, common::real32> mEntityZLayers;
        };
    }
}


