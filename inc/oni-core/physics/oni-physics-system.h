#pragma once

#include <oni-core/system/oni-system.h>
#include <oni-core/component/oni-component-physics.h>
#include <oni-core/physics/oni-physics-fwd.h>
#include <oni-core/io/oni-io-fwd.h>

namespace oni {
    class System_CarInput : public SystemTemplate<CarInput> {
    public:
        System_CarInput(EntityManager &,
                        ClientDataManager &);

    protected:
        void
        update(EntityTickContext &etc,
               CarInput &) override;

        void
        postUpdate(EntityManager &mng,
                   duration32 dt) override;

    private:
        ClientDataManager &mClientDataMng;
    };

    class System_Car : public SystemTemplate<
            Car,
            CarInput,
            CarConfig,
            WorldP3D,
            Orientation> {
    public:
        explicit System_Car(EntityManager &);

    protected:
        void
        update(EntityTickContext &,
               Car &,
               CarInput &,
               CarConfig &,
               WorldP3D &,
               Orientation &) override;

        void
        postUpdate(EntityManager &mng,
                   duration32 dt) override;
    };

    class System_SyncPos : public SystemTemplate<
            PhysicalBody,
            WorldP3D,
            Orientation,
            Scale> {
    public:
        explicit System_SyncPos(EntityManager &);

    protected:
        void
        update(EntityTickContext &,
               PhysicalBody &,
               WorldP3D &,
               Orientation &,
               Scale &) override;

        void
        postUpdate(EntityManager &mng,
                   duration32 dt) override;
    };

    class System_TimeToLive : public SystemTemplate<TimeToLive> {
    public:
        explicit System_TimeToLive(EntityManager &);

    protected:
        void
        update(EntityTickContext &,
               TimeToLive &) override;

        void
        postUpdate(EntityManager &mng,
                   duration32 dt) override;
    };

    class System_SplatOnRest : public SystemTemplate<
            SplatOnRest,
            PhysicalBody,
            Scale,
            WorldP3D,
            Orientation> {
    public:
        explicit System_SplatOnRest(EntityManager &);

    protected:
        void
        update(EntityTickContext &,
               SplatOnRest &,
               PhysicalBody &,
               Scale &,
               WorldP3D &,
               Orientation &) override;

        void
        postUpdate(EntityManager &mng,
                   duration32 dt) override;
    };

    class System_JetForce : public SystemTemplate<
            JetForce,
            Orientation,
            PhysicalBody> {
    public:
        explicit System_JetForce(EntityManager &);

    protected:
        void
        update(EntityTickContext &,
               JetForce &,
               Orientation &,
               PhysicalBody &) override;

        void
        postUpdate(EntityManager &mng,
                   duration32 dt) override;

    private:
        std::vector<EntityID> mExpiredForces{};
    };

    class System_CarCollision : public SystemTemplate<
            Car,
            PhysicalBody,
            CarInput,
            Orientation,
            WorldP3D,
            WorldP3D_History> {
    public:
        explicit System_CarCollision(EntityManager &);

    protected:
        void
        update(EntityTickContext &,
               Car &,
               PhysicalBody &,
               CarInput &,
               Orientation &,
               WorldP3D &,
               WorldP3D_History &) override;

        void
        postUpdate(EntityManager &mng,
                   duration32 dt) override;
    };

    class System_PositionAndVelocity
            : public SystemTemplate<Velocity, Acceleration, WorldP3D, Direction> {
    public:
        explicit System_PositionAndVelocity(oni::EntityManager &);

    protected:
        void
        update(EntityTickContext &,
               Velocity &,
               Acceleration &,
               WorldP3D &,
               Direction &) override;

        void
        postUpdate(EntityManager &mng,
                   duration32 dt) override;
    };

}