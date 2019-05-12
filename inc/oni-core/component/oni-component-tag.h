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

        struct Tag_LeavesMark {
        };

        struct Tag_SplatOnDeath {
        };

        struct Tag_Particle {
        };

        /// Sim
        struct Tag_SimModeServer {
        };

        struct Tag_SimModeClient {
        };

        /// Network
        struct Tag_RequiresNetworkSync {
        };

        struct Tag_OnlyComponentUpdate {
        };

        /// Physics
        struct Tag_ShaderOnlyParticlePhysics {
            // NOTE: Engine does not track placement, shader figures it out given particle age
        };

        struct Tag_EngineOnlyParticlePhysics {
            // NOTE: Engine tracks placement, shader only draws them just like sprites
        };
    }
}
