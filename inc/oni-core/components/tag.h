#pragma once

namespace oni {
    namespace components {
        struct Tag_Static {
            template<class Archive>
            void serialize(Archive &archive) {}
        };

        struct Tag_Dynamic {
            template<class Archive>
            void serialize(Archive &archive) {}
        };

        struct Tag_Vehicle {
            template<class Archive>
            void serialize(Archive &archive) {}
        };

        struct Tag_ColorShaded {
            template<class Archive>
            void serialize(Archive &archive) {}
        };

        struct Tag_TextureShaded {
            template<class Archive>
            void serialize(Archive &archive) {}
        };

        struct Tag_NewEntity {
            template<class Archive>
            void serialize(Archive &archive) {}
        };

        struct Tag_OnlyComponentUpdate {
            template<class Archive>
            void serialize(Archive &archive) {}
        };
    }
}
