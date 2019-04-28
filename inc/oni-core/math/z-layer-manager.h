#pragma once

#include <unordered_map>

#include <oni-core/component/visual.h>
#include <oni-core/component/entity.h>

namespace oni {
    namespace math {
        class ZLayerManager {
        public:
            ZLayerManager();

            explicit ZLayerManager(const component::ZLayer &);

            common::real32
            getZForEntity(component::EntityType) const;

            common::real32
            getNextZAtLayer(component::ZLayerDef);

            component::ZLayer
            getZLayer() const;

        private:
            void
            constructEntityLayers();

        private:
            const common::real32 mMajorLayerDelta{0.1f};
            const common::real32 mMinorLayerDelta{0.001f};

            component::ZLayer mZLayer{};
            component::ZLayer mZLayerTop{};

            std::unordered_map<component::EntityType, common::real32> mEntityZLayers;
        };
    }
}


