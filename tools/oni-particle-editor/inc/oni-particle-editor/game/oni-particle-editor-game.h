#pragma once

#include <oni-core/game/oni-game.h>

namespace oni {
    class ParticleEditorGame : public Game {
    public:
        ParticleEditorGame();

    protected:
        bool
        shouldTerminate() override;

        void
        _sim(r64 simTime) override;

        void
        _render(r64 simTime) override;

        void
        _display() override;

        void
        _poll() override;

        void
        _finish() override;

        void
        showFPS(i16 i161) override;

        void
        showSPS(i16 i161) override;

        void
        showPPS(i16 i161) override;

        void
        showRT(i16 i161) override;

        void
        showST(i16 i161) override;

        void
        showPT(i16 i161) override;
    };
}