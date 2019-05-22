#pragma once

#include <map>
#include <deque>
#include <chrono>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/util/oni-util-timer.h>
#include <oni-core/component/oni-component-visual.h>

namespace oni {
    namespace entities {
        class EntityManager;
    }

    namespace math {
        class ZLayerManager;
    }

    namespace gameplay {
        struct CarLapInfo {
            common::EntityID entityID{0};
            common::u16 lap{0};
            common::u32 lapTimeS{0};
            common::u32 bestLapTimeS{0};
        };

        class LapTracker {
        public:
            explicit LapTracker(entities::EntityManager &entityManager,
                                const math::ZLayerManager &);

            ~LapTracker();

            void
            tick();

        private:
            void
            resetPlayerCheckpoints(common::EntityID carEntity);

            // TODO: Decide what to track players with, PeerID? Car? EntityID?
            void
            addNewPlayer(common::EntityID carEntity);

        private:
            entities::EntityManager &mEntityManager;
            std::deque<component::Rectangle> mInitialCheckpoints{};
            std::map<common::EntityID, std::deque<component::Rectangle>> mRemainingCheckpoints{};
            std::map<common::EntityID, std::chrono::seconds> mBestLaps{};
            std::map<common::EntityID, utils::Timer> mTimers{};

            const math::ZLayerManager &mZLayerManager;
        };
    }
}