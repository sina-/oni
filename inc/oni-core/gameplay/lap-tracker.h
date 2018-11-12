#pragma once

#include <map>
#include <deque>

#include <oni-core/common/typedefs.h>
#include <oni-core/components/geometry.h>

namespace oni {
    namespace entities {
        class EntityManager;
    }

    namespace gameplay {
        class LapTracker {
        public:
            explicit LapTracker(entities::EntityManager &entityManager);

            ~LapTracker();

            void tick();

        private:
            void resetPlayerCheckpoints(common::EntityID carEntity);

            // TODO: Decide what to track players with, PeerID? Car? EntityID?
            void addNewPlayer(common::EntityID carEntity);

        private:
            entities::EntityManager &mEntityManager;
            std::deque<components::Shape> mInitialCheckpoints{};
            std::map<common::EntityID, std::deque<components::Shape>> mRemainingCheckpoints{};
        };
    }
}