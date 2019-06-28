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
            float wid;
            float px, py, nx, ny;

            wid = 0.04 - f->velocity.current;
            wid = wid * 1.5;
            if (wid < 0.00001)
                wid = 10.00001;
            delx = f->heading.x * wid;
            dely = f->heading.y * wid;

            px = f->last.x;
            py = f->last.y;
            nx = f->current.x;
            ny = f->current.y;

            auto a = component::WorldP2D{px + f->lastDelta.x, py + f->lastDelta.y};
            f->vertices.push_back(a);

            auto b = component::WorldP2D{px - f->lastDelta.x, py - f->lastDelta.y};
            f->vertices.push_back(b);

            auto c = component::WorldP2D{nx - delx, ny - dely};
            f->vertices.push_back(c);

            auto d = component::WorldP2D{nx + delx, ny + dely};
            f->vertices.push_back(d);

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

            f->lastDelta.x = delx;
            f->lastDelta.y = dely;
        }

        void
        filtersetpos(BrushTrail *f,
                     float x,
                     float y) {
            f->current.x = x;
            f->current.y = y;
            f->last.x = x;
            f->last.y = y;
            f->velocity2d.x = 0.0;
            f->velocity2d.y = 0.0;
            f->acceleration2d.x = 0.0;
            f->acceleration2d.y = 0.0;
            f->acceleration.current = 0.0;
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
            fx = mx - f->current.x;
            fy = my - f->current.y;
            f->acceleration.current = sqrt(fx * fx + fy * fy);
            if (f->acceleration.current < 0.000001) {
                return 0;
            }
            f->acceleration2d.x = fx / mass;
            f->acceleration2d.y = fy / mass;

            /* calculate new velocity */
            f->velocity2d.x += f->acceleration2d.x;
            f->velocity2d.y += f->acceleration2d.y;

            f->velocity.current = sqrt(f->velocity2d.x * f->velocity2d.x + f->velocity2d.y * f->velocity2d.y);
            f->heading.x = -f->velocity2d.y;
            f->heading.y = f->velocity2d.x;
            if (f->velocity.current < 0.000001) {
                return 0;
            }

            /* calculate angle of drawing tool */
            f->heading.x /= f->velocity.current;
            f->heading.y /= f->velocity.current;
//            if (f->fixedangle) {
//                f->angx = 0.6;
//                f->angy = 0.2;
//            }

            /* apply drag */
            f->velocity2d.x = f->velocity2d.x * (1.0 - drag);
            f->velocity2d.y = f->velocity2d.y * (1.0 - drag);

            /* update position */
            f->last.x = f->current.x;
            f->last.y = f->current.y;

            f->current.x += f->velocity2d.x;
            f->current.y += f->velocity2d.y;
            return 1;
        }

        void
        render(BrushTrail *f) {
           glBegin(GL_TRIANGLES);
            for (common::size i = 0; i + 4 < f->vertices.size(); ) {
                // a
                glVertex2f(f->vertices[i].x, f->vertices[i].y);

                // b
                ++i;
                glVertex2f(f->vertices[i].x, f->vertices[i].y);

                // c
                ++i;
                glVertex2f(f->vertices[i].x, f->vertices[i].y);

                // a
                --i;
                --i;
                glVertex2f(f->vertices[i].x, f->vertices[i].y);

                // c
                ++i;
                ++i;
                glVertex2f(f->vertices[i].x, f->vertices[i].y);

                // d
                ++i;
                glVertex2f(f->vertices[i].x, f->vertices[i].y);

                ++i;

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
