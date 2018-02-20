#pragma once

#include <vector>
#include <bitset>
#include <limits>
#include <stack>

#include <components/render-components.h>
#include <components/component.h>
#include <entities/entity.h>

namespace oni {
    namespace entities {
        class World {
        public:
            // When there is no more space in the world reserve this many slots for entities.
            static const auto EXPANSION_FACTOR = 8192;


        private:
            std::stack<unsigned long> m_FreeEntitySlots;
            /**
             * For each entity, define a bitset showing
             * which components are enabled. For example, a simple static
             * game object could be defined by (Components:SPRITE | Components:NAME).
             */
            components::EntityMask m_Entities;

            std::vector<components::Position> m_Positions;
            std::vector<components::Appearance> m_Appearances;

            /**
             * Suitable for creating entities sporadically. It can reuse free slots created by destroyEntity().
             * If there is no free slot and all the initial slots, marked by World(initialEntityCount), are taken
             * it will reallocate and add extra space based on EXPANSION_FACTOR.
             *
             * This function guarantees the returned ID is a valid memory location.
             *
             * @return entity ID.
             */
            auto createEntity();

        public:
            /**
             * Struct of lists that houses all the objects in a game based on Data Oriented Design that
             * keeps data in a contiguous stack-based memory. This is in contrast to the usual list of struct.
             *
             * When adding batch of entities, first reserve the number of entities needed by calling
             * World::reserveEntity(BATCH_SIZE);
             *
             */
            World() = default;

            void reserveEntity(unsigned long count);
            void destroyEntity(unsigned long entity);

            unsigned long addSprite(const components::Position &position, const components::Appearance &color);

            const components::EntityMask &getEntities() const;
            const components::Mask &getEntity(unsigned long entity) const;

            const components::Position &getPosition(unsigned long entity) const;
            const components::Appearance & getAppearance(unsigned long entity) const;

        };
    }
}
