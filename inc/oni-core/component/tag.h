#pragma once

namespace oni {
    namespace component {
        struct Tag_Static {
        };

        struct Tag_Dynamic {
        };

        struct Tag_ColorShaded {
        };

        struct Tag_TextureShaded {
        };

        struct Tag_Audible {
        };

        struct Tag_LeavesMark {
        };

        // NOTE: Engine does not track placement, shader figures it out given particle age
        struct Tag_ShaderOnlyParticlePhysics {
        };

        // NOTE: Engine tracks placement, shader only draws them just like sprites
        struct Tag_EngineOnlyParticlePhysics {
        };

        struct Tag_Particle {
        };

        struct Tag_RequiresNetworkSync {
        };

        struct Tag_OnlyComponentUpdate {
        };
    }
}
