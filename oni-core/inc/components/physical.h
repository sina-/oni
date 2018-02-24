#pragma once

#include <math/vec2.h>
#include <math/vec3.h>
#include <math/vec4.h>
#include <math/mat4.h>

namespace oni {
    namespace components {
        struct Placement {
            /**
             *    B    C
             *    +----+
             *    |    |
             *    +----+
             *    A    D
             */
            math::vec3 vertexA;
            math::vec3 vertexB;
            math::vec3 vertexC;
            math::vec3 vertexD;

            Placement() : vertexA(math::vec3()), vertexB(math::vec3()),
                          vertexC(math::vec3()), vertexD(math::vec3()) {}

            Placement(const math::vec3 &m_PositionA, const math::vec3 &m_PositionB,
                      const math::vec3 &m_PositionC, const math::vec3 &m_PositionD) :
                    vertexA(m_PositionA),
                    vertexB(m_PositionB),
                    vertexC(m_PositionC),
                    vertexD(m_PositionD) {}

            Placement(const Placement &other) = default;

/*            void setVertexPositions(const math::vec2 &size, const math::vec3 &pos) {
        m_PositionA = pos;
        vertexB = math::vec3(pos.x, pos.y + size.y, pos.z);
        vertexC = math::vec3(pos.x + size.x, pos.y + size.y, pos.z);
        vertexD = math::vec3(pos.x + size.x, pos.y, pos.z);
    }*/
        };

        struct Velocity {
            math::vec3 direction;
            float magnitude;

            Velocity() : direction(math::vec3()), magnitude(0.0f) {};

            Velocity(const math::vec3 &_direction, float _magnitude) : direction(_direction),
                                                                       magnitude(_magnitude) {};

            Velocity(const Velocity &other) = default;

        };


    }
}