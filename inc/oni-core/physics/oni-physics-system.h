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
            Tag_SplatOnRest,
            PhysicalBody,
            Scale,
            EntityAssetsPack,
            WorldP3D,
            Orientation> {
    public:
        explicit System_SplatOnRest(EntityManager &);

    protected:
        void
        update(EntityTickContext &,
               Tag_SplatOnRest &,
               PhysicalBody &,
               Scale &,
               EntityAssetsPack &,
               WorldP3D &,
               Orientation &) override;

        void
        postUpdate(EntityManager &mng,
                   duration32 dt) override;
    };
}