#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/entities/oni-entities-manager.h>

namespace oni {
    class System {
    public:
        explicit System(EntityManager &mng) : mEntityManager(mng) {}

        virtual ~System() = default;

        void
        tick(duration32 dt) {
            update(mEntityManager, dt);
            postUpdate(mEntityManager, dt);
        }

    protected:
        virtual void
        update(EntityManager &mng,
               duration32 dt) = 0;

        virtual void
        postUpdate(EntityManager &mng,
                   duration32 dt) = 0;

    private:
        EntityManager &mEntityManager;
    };

    template<class ...Component>
    class SystemTemplate : public System {
    protected:
        explicit SystemTemplate(EntityManager &mng) : System(mng) {}

        void
        update(EntityManager &mng,
               duration32 dt) override {
            auto functor = [this](EntityTickContext &etc,
                                  Component &... args) {
                update(etc, args...);
            };
            mng.update<Component...>(functor, dt);
        }

        virtual void
        update(EntityTickContext &,
               Component &...args) = 0;
    };
}