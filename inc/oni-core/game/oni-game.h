#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/util/oni-util-timer.h>

namespace oni {
    class Game {
    public:
        Game();

        Game(u16 tickRate);

        virtual ~Game();

        void
        run();

    protected:
        r32
        getTickFrequency();

        virtual bool
        shouldTerminate() = 0;

        virtual void
        initRenderer();

        virtual void
        initSystems();

        virtual void
        _sim(r64 simTime) = 0;

        virtual void
        _render(r64 simTime) = 0;

        virtual void
        _display() = 0;

        virtual void
        _poll() = 0;

        virtual void
        _finish() = 0;

        virtual void
        showFPS(i16) = 0;

        virtual void
        showSPS(i16) = 0;

        virtual void
        showPPS(i16) = 0;

        virtual void
        showRT(i16) = 0;

        virtual void
        showST(i16) = 0;

        virtual void
        showPT(i16) = 0;

    private:
        void
        sim();

        void
        render();

        void
        display();

        void
        poll();

        void
        finish();

    private:
        // 60Hz
        const r32 mTickS{1 / 60.0f};

    protected:
        Timer mSimTimer{};
        Timer mSimLoopTimer{};
        u16 mSimUpdateCounter{0};
        r64 mSimMS{0.0f};

        Timer mPollTimer{};
        Timer mPollLoopTimer{};
        u16 mPollUpdateCounter{0};
        r64 mPollMS{0.0f};

        Timer mRenderTimer{};
        Timer mRenderLoopTimer{};
        u16 mRenderUpdateCounter{0};
        r64 mRenderMS{0.0f};

        Timer mGameLoopTimer{};
    };
}
