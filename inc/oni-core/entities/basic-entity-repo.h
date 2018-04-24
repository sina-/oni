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
            virtual common::entityID _createEntity();

        public:
            /**
             * Struct of lists that houses all the objects in a game based on Data Oriented Design that
             * keeps data in a contiguous stack-based memory. This is in contrast to the usual list of struct.
             *
             */
            BasicEntityRepo();

            virtual ~BasicEntityRepo() = default;

            void reserveEntity(size_t count);

            void destroyEntity(common::entityID id);

            common::entityID createEntity(const components::Mask &mask, const components::ShaderID &shaderID);

            void addComponent(common::entityID id, const components::Component &component);

            void setEntityPlacementLocal(common::entityID id, const components::Placement &placement);

            void setEntityPlacementWorld(common::entityID id, const components::Placement &placement);

            void setEntityAppearance(common::entityID id, const components::Appearance &appearance);

            void setEntityTexture(common::entityID id, const components::Texture &texture);

            void setEntityShaderID(common::entityID id, const components::ShaderID &shaderID);

            void setEntityText(common::entityID id, const components::Text &text);

            const components::EntityMasks &getEntities() const;

            const components::Mask &getEntity(common::entityID id) const;

            const components::Placement &getEntityPlacementLocal(common::entityID id) const;

            const components::Placement &getEntityPlacementWorld(common::entityID id) const;

            components::Placement &getEntityPlacementLocal(common::entityID id);

            components::Placement &getEntityPlacementWorld(common::entityID id);

            const components::Appearance &getEntityAppearance(common::entityID id) const;

            const components::Texture &getEntityTexture(common::entityID id) const;

            const components::ShaderID &getEntityShaderID(common::entityID id) const;

            const components::Text &getEntityText(common::entityID id) const;

            components::Text &getEntityText(common::entityID id);

        protected:
            std::stack<common::entityID> mFreeEntitySlots{};
            /**
             * For each entity, define a bitset showing
             * which components are enabled. For example, a simple static
             * game object could be defined by (Components:Vertex | Components:Appearance).
             */
            components::EntityMasks mEntities{};

            std::vector<components::Placement> mPlacementsLocal{};
            std::vector<components::Placement> mPlacementsWorld{};
            std::vector<components::Appearance> mAppearances{};
            std::vector<components::Texture> mTextures{};
            std::vector<components::ShaderID> mShaderIDs{};

            // TODO: Does this have to be here? Maybe create a new repo that handles
            // font data. And find a better name than Text.
            std::vector<components::Text> mTexts{};
        };
    }
}
