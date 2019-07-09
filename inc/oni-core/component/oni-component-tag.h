#pragma once

namespace oni {
    namespace component {
        /// Audio
        struct Tag_Audible {
        };

        /// Graphics
        struct Tag_Static {
        };

        struct Tag_Dynamic {
        };

        struct Tag_ColorShaded {
        };

        struct Tag_TextureShaded {
        };

        struct Tag_SplatOnDeath {
        };

        /// Sim
        struct Tag_SimServerSideOnly {
        };

        struct Tag_SimClientSideOnly {
        };

        /// Network
        struct Tag_NetworkSyncEntity {
        };

        struct Tag_NetworkSyncComponent {
        };
    }
}
