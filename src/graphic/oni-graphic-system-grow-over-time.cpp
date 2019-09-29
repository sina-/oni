#include <oni-core/graphic/oni-graphic-system.h>

namespace oni {
    System_GrowOverTime::System_GrowOverTime(EntityManager &em) : SystemTemplate(em) {}

    void
    System_GrowOverTime::update(EntityTickContext &etc,
                                GrowOverTime &growth,
                                Scale &scale) {
        assert(etc.mng.getSimMode() == SimMode::CLIENT ||
               etc.mng.getSimMode() == SimMode::CLIENT_SIDE_SERVER);

        auto doneGrowing = std::vector<EntityID>();
        bool doneGrowingX = false;
        bool doneGrowingY = false;

        growth.elapsed += etc.dt;
        if (almost_Greater(growth.elapsed, growth.period)) {
            if (almost_Less(scale.x, growth.maxSize.x)) {
                scale.x += growth.factor;
            } else {
                doneGrowingX = true;
            }

            if (almost_Less(scale.y, growth.maxSize.y)) {
                scale.y += growth.factor;
            } else {
                doneGrowingY = true;
            }

            if (doneGrowingX && doneGrowingY) {
                doneGrowing.emplace_back(etc.id);
            }
            growth.elapsed = 0.f;
        }
    }

    void
    System_GrowOverTime::postUpdate(EntityManager &mng,
                                    duration32 dt) {
        for (auto &&id: mExpiredComponents) {
            mng.removeComponent<GrowOverTime>(id);
        }
        mExpiredComponents.clear();
    }
}