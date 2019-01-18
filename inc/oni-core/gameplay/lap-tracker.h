#pragma once

#include <map>
#include <deque>
#include <chrono>

#include <oni-core/common/typedefs.h>
#include <oni-core/components/geometry.h>
#include <oni-core/utils/timer.h>
#include <oni-core/components/visual.h>

namespace oni {
    namespace entities {
        class EntityManager;
    }

    namespace gameplay {
        class LapTracker {
        public:
            explicit LapTracker(entities::EntityManager &entityManager, const components::ZLevel&);

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
            std::map<common::EntityID, std::chrono::seconds> mBestLaps{};
            std::map<common::EntityID, utils::Timer> mTimers{};
            components::ZLevel mZLevel{};
        };
    }
}