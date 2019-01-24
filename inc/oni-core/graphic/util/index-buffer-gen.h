#pragma once

namespace oni {
    namespace graphic {


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
        template<class T>
        class IndexBufferGen {
            T mCounter{0};
            T mOffset{0};
        public:
            IndexBufferGen() = default;

            T getValue() {
                T current = mOffset;

                switch (mCounter % 6) {
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

                mCounter++;

                if (mCounter % 6 == 0) {
                    mOffset += 4;
                }

                return current;
            }

            T operator()() {
                return getValue();
            }

        };
    }
}