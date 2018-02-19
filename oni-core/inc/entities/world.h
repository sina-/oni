#pragma once

#include <vector>
#include <bitset>
#include <limits>
#include <stack>

#include <components/render-components.h>
#include <components/component.h>

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

            std::vector<components::Renderable2D> m_Renderables;

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
             * @param initialEntityCount initial number of objects in the world. 2 ^ 16 is a good start.
             */
            explicit World(unsigned long initialEntityCount);

            void destroyEntity(unsigned long entity);

            unsigned long addCar(const components::Renderable2D &renderable2D);

            const components::EntityMask &getEntities() const;
            const components::Mask &getEntity(unsigned long entity) const;

            const components::Renderable2D &getRenderable2D(unsigned long entity) const;

        };
    }
}
