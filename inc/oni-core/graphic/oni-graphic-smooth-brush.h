#pragma once

namespace oni {
    namespace graphic {
#define MAXPOLYS    (4 * 2 * 10000)
        struct BrushTrail {
            float curx, cury;
            float velx, vely, vel;
            float accx, accy, acc;
            float angx, angy;
            float mass, drag;
            float lastx, lasty;
            float lastdelx, lastdely;
            float polyverts[MAXPOLYS];
            int nsegs = 0;
        };

        void
        testDraw(BrushTrail *f,
                 float x,
                 float y);

        void
        filtersetpos(BrushTrail *f,
                     float x,
                     float y);
    }
}

