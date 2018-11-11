#include <oni-core/gameplay/lap-tracker.h>

#include <oni-core/entities/entity-manager.h>
#include <oni-core/physics/transformation.h>
#include <oni-core/physics/collision.h>

namespace oni {
    namespace gameplay {

        LapTracker::LapTracker(entities::EntityManager &entityManager) : mEntityManager(entityManager) {
            auto checkpoint1 = components::Shape::fromPositionAndSize(math::vec3{70, -40}, math::vec2{20, 20});
            auto checkpoint2 = components::Shape::fromPositionAndSize(math::vec3{70, 30}, math::vec2{20, 20});
            auto checkpoint3 = components::Shape::fromPositionAndSize(math::vec3{-80, 30}, math::vec2{20, 20});
            auto checkpoint4 = components::Shape::fromPositionAndSize(math::vec3{-80, -40}, math::vec2{20, 20});

            mInitialCheckpoints.push_back(checkpoint4);
            mInitialCheckpoints.push_back(checkpoint3);
            mInitialCheckpoints.push_back(checkpoint2);
            mInitialCheckpoints.push_back(checkpoint1);
        }

        LapTracker::~LapTracker() = default;;

        void LapTracker::tick() {
            auto carView = mEntityManager.createViewScopeLock<components::Shape, components::Placement, components::Car>();
            for (auto &&entity: carView) {

                // TODO: This is obsolete if registry is only modified from one thread. Then I can expose addNewPlayer()
                // and let the new play routine handle player additions and just have an assert here.
                if (mRemainingCheckpoints.find(entity) == mRemainingCheckpoints.end()) {
                    addNewPlayer(entity);
                }

                const auto &nextCheckpoint = mRemainingCheckpoints[entity].back();
                auto carShapeWorld = carView.get<components::Shape>(entity);
                auto entityPlacement = carView.get<components::Placement>(entity);
                physics::Transformation::localToWorldTranslation(entityPlacement.position, carShapeWorld);

                if (physics::collides(nextCheckpoint, carShapeWorld)) {
                    mRemainingCheckpoints[entity].pop_back();
                    std::cout << "Remaining checkpoints: " << mRemainingCheckpoints[entity].size() << "\n";
                }

                if (mRemainingCheckpoints[entity].empty()) {
                    ++mPlayerLaps[entity];
                    std::cout << "Laps compeleted: " << mPlayerLaps[entity] << "\n";
                    resetPlayerCheckpoints(entity);
                }
            }
        }

        void LapTracker::addNewPlayer(common::EntityID carEntity) {
            mPlayerLaps[carEntity] = 0;
            resetPlayerCheckpoints(carEntity);
        }

        void LapTracker::resetPlayerCheckpoints(common::EntityID carEntity) {
            mRemainingCheckpoints[carEntity] = mInitialCheckpoints;
        }
    }
}