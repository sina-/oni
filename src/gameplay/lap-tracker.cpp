#include <oni-core/gameplay/lap-tracker.h>

#include <oni-core/components/gameplay.h>
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
            std::vector<common::EntityID> entitiesToUpdate;
            {
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
                        entitiesToUpdate.push_back(entity);
                    }
                }
            }

            if (!entitiesToUpdate.empty()) {
                // NOTE: I don't need the components::Car but Entt requires at least two components for a view
                // for some freaking reason.
                auto carLapView = mEntityManager.createViewScopeLock<components::CarLap, components::Car>();
                for (auto &&entity: entitiesToUpdate) {
                    auto& carLap = carLapView.get<components::CarLap>(entity);
                    ++carLap.lap;
                    std::cout << "Laps compeleted: " << carLap.lap << "\n";
                    mEntityManager.accommodate<components::TagOnlyComponentUpdate>(entity);
                    resetPlayerCheckpoints(entity);
                }

            }
        }

        void LapTracker::addNewPlayer(common::EntityID carEntity) {
            auto carLap = components::CarLap{};
            carLap.entityID = carEntity;
            carLap.lap = 0;
            mEntityManager.assign<components::CarLap>(carEntity, carLap);
            mEntityManager.accommodate<components::TagOnlyComponentUpdate>(carEntity);
            resetPlayerCheckpoints(carEntity);
        }

        void LapTracker::resetPlayerCheckpoints(common::EntityID carEntity) {
            mRemainingCheckpoints[carEntity] = mInitialCheckpoints;
        }
    }
}