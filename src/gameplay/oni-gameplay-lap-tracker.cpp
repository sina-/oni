#include <oni-core/gameplay/oni-gameplay-lap-tracker.h>

#include <oni-core/math/oni-math-z-layer-manager.h>
#include <oni-core/component/oni-component-gameplay.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/math/oni-math-transformation.h>
#include <oni-core/math/oni-math-intersects.h>
#include <oni-core/component/oni-component-physic.h>

namespace oni {
    namespace gameplay {

        LapTracker::LapTracker(entities::EntityManager &entityManager,
                               const math::ZLayerManager &zLayerManager)
                : mEntityManager(entityManager), mZLayerManager(zLayerManager) {

            auto checkpoint1 = component::Shape::fromPositionAndSize(math::vec3{70, -40}, math::vec2{20, 20});
            auto checkpoint2 = component::Shape::fromPositionAndSize(math::vec3{70, 30}, math::vec2{20, 20});
            auto checkpoint3 = component::Shape::fromPositionAndSize(math::vec3{-80, 30}, math::vec2{20, 20});
            auto checkpoint4 = component::Shape::fromPositionAndSize(math::vec3{-80, -40}, math::vec2{20, 20});

            mInitialCheckpoints.push_back(checkpoint4);
            mInitialCheckpoints.push_back(checkpoint3);
            mInitialCheckpoints.push_back(checkpoint2);
            mInitialCheckpoints.push_back(checkpoint1);
        }

        LapTracker::~LapTracker() = default;;

        void
        LapTracker::tick() {
            std::vector<common::EntityID> entitiesToUpdate;
            {
                auto carView = mEntityManager.createView<component::Shape, component::WorldP3D, component::Car>();
                for (auto &&entity: carView) {

                    // TODO: This is obsolete if registry is only modified from one thread. Then I can expose addNewPlayer()
                    // and let the new play routine handle player additions and just have an assert that mRemainingCheckpoints
                    // has this entity.
                    // I can't addNewPlayer from server-game because player creation and laptracker tick is called from
                    // different threads and laptracker is not thread safe.
                    if (mRemainingCheckpoints.find(entity) == mRemainingCheckpoints.end()) {
                        addNewPlayer(entity);
                    }

                    const auto &nextCheckpoint = mRemainingCheckpoints[entity].back();
                    auto carShapeWorld = carView.get<component::Shape>(entity);
                    auto entityPlacement = carView.get<component::WorldP3D>(entity);
                    math::localToWorldTranslation(entityPlacement, carShapeWorld);

                    if (math::intersects(nextCheckpoint, carShapeWorld)) {
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
                auto carLapView = mEntityManager.createView<gameplay::CarLapInfo, component::Car>();
                for (auto &&entity: entitiesToUpdate) {
                    auto currentTime = mTimers[entity].elapsed<std::chrono::seconds>();
                    auto currentBest = mBestLaps[entity];
                    if (currentTime < currentBest) {
                        mBestLaps[entity] = currentTime;
                    }
                    std::cout << "Lap time: " << currentTime.count() << "s" << "\n";
                    std::cout << "Current best time: " << mBestLaps[entity].count() << "s" << "\n";

                    auto &carLap = carLapView.get<gameplay::CarLapInfo>(entity);
                    ++carLap.lap;
                    carLap.bestLapTimeS = static_cast<common::u32>(mBestLaps[entity].count());
                    carLap.lapTimeS = static_cast<common::u32>(currentTime.count());

                    std::cout << "Laps completed: " << carLap.lap << "\n";
                    mEntityManager.tagForComponentSync(entity);

                    resetPlayerCheckpoints(entity);
                }
            }
        }

        void
        LapTracker::addNewPlayer(common::EntityID carEntity) {
            mBestLaps[carEntity] = std::chrono::hours(666);
            mTimers[carEntity] = utils::Timer();
            resetPlayerCheckpoints(carEntity);
        }

        void
        LapTracker::resetPlayerCheckpoints(common::EntityID carEntity) {
            mRemainingCheckpoints[carEntity] = mInitialCheckpoints;
            mTimers[carEntity].restart();
        }
    }
}