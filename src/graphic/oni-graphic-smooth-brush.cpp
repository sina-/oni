/*
 *	Inspired by dynadraw -
 *	http://www.graficaobscura.com/dyna/index.html
 *
 */

#include<oni-core/graphic/oni-graphic-smooth-brush.h>

#include <cassert>
#include <math.h>

#include <GL/glew.h>

namespace oni {
    namespace graphic {

        float
        flerp(float f0,
              float f1,
              float p) {
            return ((f0 * (1.0 - p)) + (f1 * p));
        }

        void
        addSegment(BrushTrail *f) {
            float delx, dely;
            float wid, *fptr;
            float px, py, nx, ny;

            wid = 0.04 - f->vel;
            wid = wid * 1.5;
            if (wid < 0.00001)
                wid = 10.00001;
            delx = f->angx * wid;
            dely = f->angy * wid;

            px = f->lastx;
            py = f->lasty;
            nx = f->curx;
            ny = f->cury;

            fptr = f->polyverts + 8 * f->nsegs;
            ///
            fptr[0] = px + f->lastdelx;
            fptr[1] = py + f->lastdely;

            fptr += 2;
            fptr[0] = px - f->lastdelx;
            fptr[1] = py - f->lastdely;

            fptr += 2;
            fptr[0] = nx - delx;
            fptr[1] = ny - dely;

            fptr += 2;
            fptr[0] = nx + delx;
            fptr[1] = ny + dely;
            ///

            f->nsegs++;
            if (f->nsegs >= MAXPOLYS) {
                assert(false);
            }

//            ///
//            glBegin(GL_LINE_LOOP);
//            fptr -= 8;
//
//            fptr += 2;
//            glVertex2fv(fptr);
//
//            fptr += 2;
//            glVertex2fv(fptr);
//
//            fptr += 2;
//            glVertex2fv(fptr);
//
//            fptr += 2;
//            glVertex2fv(fptr);
//
//            glEnd();
//            ///

            f->lastdelx = delx;
            f->lastdely = dely;
        }


        void
        filtersetpos(BrushTrail *f,
                     float x,
                     float y) {
            f->curx = x;
            f->cury = y;
            f->lastx = x;
            f->lasty = y;
            f->velx = 0.0;
            f->vely = 0.0;
            f->accx = 0.0;
            f->accy = 0.0;
        }

        int
        filterapply(BrushTrail *f,
                    float curmass,
                    float curdrag,
                    float mx,
                    float my) {
            float mass, drag;
            float fx, fy;

            /* calculate mass and drag */
            mass = 1;//flerp(1.0, 160.0, curmass);
            drag = 0.6;//flerp(0.00, 0.5, curdrag * curdrag);

            /* calculate force and acceleration */
            fx = mx - f->curx;
            fy = my - f->cury;
            f->acc = sqrt(fx * fx + fy * fy);
            if (f->acc < 0.000001) {
                return 0;
            }
            f->accx = fx / mass;
            f->accy = fy / mass;

            /* calculate new velocity */
            f->velx += f->accx;
            f->vely += f->accy;
            f->vel = sqrt(f->velx * f->velx + f->vely * f->vely);
            f->angx = -f->vely;
            f->angy = f->velx;
            if (f->vel < 0.000001) {
                return 0;
            }

            /* calculate angle of drawing tool */
            f->angx /= f->vel;
            f->angy /= f->vel;
//            if (f->fixedangle) {
//                f->angx = 0.6;
//                f->angy = 0.2;
//            }

            /* apply drag */
            f->velx = f->velx * (1.0 - drag);
            f->vely = f->vely * (1.0 - drag);

            /* update position */
            f->lastx = f->curx;
            f->lasty = f->cury;

            f->curx += f->velx;
            f->cury += f->vely;
            return 1;
        }

        void
        render(BrushTrail *f) {
            auto fptr = f->polyverts;
            glShadeModel(GL_FLAT);
            glBegin(GL_TRIANGLES);
            for (int i = 0; i < f->nsegs; ++i) {
                /// a
                glVertex2fv(fptr);

                fptr += 2;
                /// b
                glVertex2fv(fptr);

                fptr += 2;
                /// c
                glVertex2fv(fptr);

                fptr -= 4;
                /// a
                glVertex2fv(fptr);

                fptr += 4;
                /// c
                glVertex2fv(fptr);

                fptr += 2;
                /// d
                glVertex2fv(fptr);

                fptr += 2;
            }
            glEnd();
        }

        void
        testDraw(BrushTrail *f,
                 float mx,
                 float my) {
            static bool start = true;
            if (start) {
                graphic::filtersetpos(f, mx, my);
                glOrtho(0.0, 1600, 0, 900, -1.0, 1.0);
                start = false;
                return;
            }

            float curmass, curdrag;
            curmass = 2.5;
            curdrag = 0.25;
            static double lastx = mx;
            static double lasty = my;

            auto x2 = (lastx - mx) * (lastx - mx);
            auto y2 = (lasty - my) * (lasty - my);
            auto d = std::sqrt(x2 + y2);
            if (d > 20) {
                lastx = mx;
                lasty = my;
                if (filterapply(f, curmass, curdrag, mx, my)) {
                    addSegment(f);
                }
            }
            render(f);
        }
    }
}
