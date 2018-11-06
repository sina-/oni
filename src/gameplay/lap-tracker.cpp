#include <oni-core/gameplay/lap-tracker.h>

namespace oni {
    namespace gameplay {

        LapTracker::LapTracker(entities::EntityManager &entityManager) : mEntityManager(entityManager) {
            auto checkpoint1 = components::Checkpoint{};
            auto checkpoint2 = components::Checkpoint{};
            auto checkpoint3 = components::Checkpoint{};
            auto checkpoint4 = components::Checkpoint{};

            mCheckpoints.push_back(checkpoint1);
            mCheckpoints.push_back(checkpoint2);
            mCheckpoints.push_back(checkpoint3);
            mCheckpoints.push_back(checkpoint4);
        }

        LapTracker::~LapTracker() = default;

        void LapTracker::tick() {
            // TODO: Get this list from either registry if you decide to merge client data and registry,
            // or client data manager
            std::vector<common::EntityID> carEntities;
            for (auto &&entity: carEntities) {
                const auto &topCheckpoint = mRemainingCheckpoints[entity].back();
                // IF entity collides with topCheckpoint
                {
                    mRemainingCheckpoints[entity].pop_back();
                }

                if (mRemainingCheckpoints[entity].empty()) {
                    ++mPlayerLaps[entity];
                    resetPlayerCheckpoints(entity);
                }
            }

        }

        void LapTracker::addNewPlayer(common::EntityID carEntity) {
            mPlayerLaps[carEntity] = 0;
            resetPlayerCheckpoints(carEntity);
        }

        void LapTracker::resetPlayerCheckpoints(common::EntityID carEntity) {
            mRemainingCheckpoints[carEntity] = mCheckpoints;
        }
    }
}