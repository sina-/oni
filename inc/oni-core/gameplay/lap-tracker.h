#pragma once

#include <map>
#include <deque>

#include <oni-core/common/typedefs.h>
#include <oni-core/components/physics.h>

namespace oni {
    namespace entities {
        class EntityManager;
    }

    namespace gameplay {
        class LapTracker {
        public:
            LapTracker(entities::EntityManager &entityManager);

            ~LapTracker();

            // TODO: Decide what to track players with, PeerID? Car? EntityID?
            void addNewPlayer(common::EntityID carEntity);

            void tick();

        private:
            void resetPlayerCheckpoints(common::EntityID carEntity);

        private:
            entities::EntityManager &mEntityManager;
            std::deque<components::Checkpoint> mCheckpoints{};
            std::map<common::EntityID, std::deque<components::Checkpoint>> mRemainingCheckpoints{};
            std::map<common::EntityID, common::uint8> mPlayerLaps{};
        };
    }
}