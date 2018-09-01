#pragma once

namespace oni {
    namespace components {
        struct TagNewlyCreated {
            template<class Archive>
            void serialize(Archive &archive) {}
        };

        struct TagUpdatedEntity {
            template<class Archive>
            void serialize(Archive &archive) {}
        };
    }
}
