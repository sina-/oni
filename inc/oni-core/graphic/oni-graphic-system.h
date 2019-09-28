#pragma once

#include <oni-core/system/oni-system.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/component/oni-component-geometry.h>

namespace oni {
    class System_GrowOverTime : public SystemTemplate<GrowOverTime, Scale> {
    public:
        explicit System_GrowOverTime(EntityManager &);

        void
        update(EntityTickContext &,
               GrowOverTime &,
               Scale &) override;

        void
        postUpdate(EntityManager &,
                   duration32 dt) override;

    private:
        std::vector<EntityID> mExpiredComponents{};
    };

    class System_MaterialTransition : public SystemTemplate<MaterialTransition_List, MaterialSkin> {
    public:
        explicit System_MaterialTransition(EntityManager &);

    protected:
        void
        postUpdate(EntityManager &mng,
                   duration32 dt) override;

        void
        update(EntityTickContext &context,
               MaterialTransition_List &,
               MaterialSkin &) override;

    private:
        static void
        updateTextureAnimated(MaterialTransition_Texture &,
                              r64 dt);

        static void
        updateTint(MaterialSkin &,
                   MaterialTransition_Color,
                   const TimeToLive &);

        static void
        updateFade(MaterialSkin &skin,
                   MaterialTransition_Fade &,
                   const TimeToLive &);

    };
}