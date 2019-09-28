#include <oni-core/physics/oni-physics-system.h>

namespace oni {
    System_TimeToLive::System_TimeToLive(EntityManager &em) : SystemTemplate(em) {
        assert(em.getSimMode() == SimMode::CLIENT ||
               em.getSimMode() == SimMode::SERVER);
    }

    void
    System_TimeToLive::update(EntityTickContext &etc,
                              TimeToLive &ttl) {
        ttl.currentAge += etc.dt;
        if (ttl.currentAge > ttl.maxAge) {
            if (etc.mng.has<Tag_SplatOnDeath>(etc.id)) {
                auto &pos = etc.mng.get<WorldP3D>(etc.id);
                auto &size = etc.mng.get<Scale>(etc.id);
                auto &tag = etc.mng.get<EntityAssetsPack>(etc.id);
                auto &ornt = etc.mng.get<Orientation>(etc.id);

                auto callback = [&etc]() {
                    etc.mng.deleteEntity(etc.id);
                };

                etc.mng.enqueueEvent<Event_SplatOnDeath>(pos, size, ornt, tag, std::move(callback));
            } else {
                etc.mng.markForDeletion(etc.id);
            }
        }
    }

    void
    System_TimeToLive::postUpdate(EntityManager &mng,
                                  duration32 dt) {
        mng.flushDeletions();
    }
}
