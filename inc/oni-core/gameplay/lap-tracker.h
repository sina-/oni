#pragma once

#include <map>
#include <deque>
#include <chrono>

#include <oni-core/common/typedefs.h>
#include <oni-core/component/geometry.h>
#include <oni-core/utils/timer.h>
#include <oni-core/component/visual.h>

namespace oni {
    namespace entities {
        class EntityManager;
    }

    namespace math {
        class ZLayerManager;
    }

    namespace gameplay {
        class LapTracker {
        public:
            explicit LapTracker(entities::EntityManager &entityManager, const math::ZLayerManager&);

            ~LapTracker();

            void tick();

        private:
            void resetPlayerCheckpoints(common::EntityID carEntity);

            // TODO: Decide what to track players with, PeerID? Car? EntityID?
            void addNewPlayer(common::EntityID carEntity);

        private:
            entities::EntityManager &mEntityManager;
            std::deque<component::Shape> mInitialCheckpoints{};
            std::map<common::EntityID, std::deque<component::Shape>> mRemainingCheckpoints{};
            std::map<common::EntityID, std::chrono::seconds> mBestLaps{};
            std::map<common::EntityID, utils::Timer> mTimers{};

            const math::ZLayerManager& mZLayerManager;
        };
    }
}