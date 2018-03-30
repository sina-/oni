#pragma once

#include <vector>
#include <bitset>
#include <limits>
#include <stack>

#include <oni-core/components/visual.h>
#include <oni-core/components/component.h>
#include <oni-core/entities/entity.h>
#include <oni-core/components/physical.h>

namespace oni {
    namespace entities {
        class BasicEntityRepo {
        protected:
            /**
             * It can reuse free slots created by destroyEntity().
             * When adding batch of entities, first reserve the number of entities needed by calling
             * World::reserveEntity(BATCH_SIZE);
             *
             * This function guarantees the returned ID is a valid memory location.
             *
             * @return entity ID.
             */
            virtual entities::entityID _createEntity();

        public:
            /**
             * Struct of lists that houses all the objects in a game based on Data Oriented Design that
             * keeps data in a contiguous stack-based memory. This is in contrast to the usual list of struct.
             *
             */
            BasicEntityRepo() = default;

            void reserveEntity(size_t count);

            void destroyEntity(entities::entityID id);

            entities::entityID createEntity(const components::Mask &mask, const components::LayerID &layerID);

            void addComponent(entities::entityID id, const components::Component &component);

            void setEntityPlacement(entities::entityID id, const components::Placement &placement);

            void setEntityAppearance(entities::entityID id, const components::Appearance &appearance);

            void setEntityTexture(entities::entityID id, const components::Texture &texture);

            void setEntityLayerID(entities::entityID id, const components::LayerID &tileID);

            void setEntityText(entities::entityID id, const components::Text &text);

            const components::EntityMasks &getEntities() const;

            const components::Mask &getEntity(entities::entityID id) const;

            const components::Placement &getEntityPlacement(entities::entityID id) const;

            const components::Appearance &getEntityAppearance(entities::entityID id) const;

            const components::Texture &getEntityTexture(entities::entityID id) const;

            const components::LayerID &getEntityLayerID(entities::entityID id) const;

            const components::Text &getEntityText(entities::entityID id) const;

            components::Text &getEntityText(entities::entityID id);

        protected:
            std::stack<entities::entityID> mFreeEntitySlots;
            /**
             * For each entity, define a bitset showing
             * which components are enabled. For example, a simple static
             * game object could be defined by (Components:Vertex | Components:Appearance).
             */
            components::EntityMasks mEntities;

            std::vector<components::Placement> mPlacements;
            std::vector<components::Appearance> mAppearances;
            std::vector<components::Texture> mTextures;
            std::vector<components::LayerID> mLayerIDs;
            std::vector<components::Text> mTexts;

        };
    }
}
