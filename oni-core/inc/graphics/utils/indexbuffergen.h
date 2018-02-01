#pragma once

#include <GL/glew.h>

namespace oni {
    namespace graphics {


        /*
         * Generates indices for index buffer of triangles. Used by std::generate()
         * to pre-populate a big buffer.
         *
         * Example:
            std::vector<GLuint> indices(10000);
            IndexBufferGen gen;
            std::generate(indices.begin(), indices.end(), gen);
         *
         */
        class IndexBufferGen {
            GLuint m_Counter = 0;
            GLuint m_Offset = 0;
        public:
            IndexBufferGen() = default;

            GLuint getValue() {
                GLuint current = m_Offset;

                switch (m_Counter % 6) {
                    case 0:
                        current += 0;
                        break;
                    case 1:
                        current += 1;
                        break;
                    case 2:
                        current += 2;
                        break;
                    case 3:
                        current += 2;
                        break;
                    case 4:
                        current += 3;
                        break;
                    case 5:
                        current += 0;
                        break;
                    default:
                        break;
                }

                m_Counter++;

                if (m_Counter % 6 == 0) {
                    m_Offset += 4;
                }

                return current;
            }

            GLuint operator()() {
                return getValue();
            }

        };
    }
}