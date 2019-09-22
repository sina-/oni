#pragma once

#include <memory>
#include <vector>

#include <oni-core/common/oni-common-typedefs-graphic.h>
#include <oni-core/math/oni-math-vec2.h>
#include <oni-core/math/oni-math-vec3.h>
#include <oni-core/math/oni-math-vec4.h>
#include <oni-core/math/oni-math-mat4.h>


namespace oni {
    struct TessellationVertex {
        oniGLint samplerID{-1};
        r32 ornt{0.f};
        r32 effect{0.f};
        vec2 halfSize{1.f};
        vec3 position{0.f, 0.f, 0.f};
        vec4 color{0.f, 0.f, 0.f, 0.f};
        vec2 uv_0{0, 0};
        vec2 uv_1{0, 1};
        vec2 uv_2{1, 1};
        vec2 uv_3{1, 0};
        r32 transformType{0.f};
    };

    struct StripVertex {
        vec4 center{0.f, 0.f, 0.f, 0.f};
        r32 texoff{0.f};
        vec3 bc{};
        //vec4 color{0.f, 0.f, 0.f, 0.f};
        //common::oniGLint samplerID{-1};
    };

    struct QuadVertex {
        oniGLfloat samplerFront{-1};
        oniGLfloat samplerBack{-1};
        vec4 color{0.f, 0.f, 0.f, 0.f};
        vec2 uv{};
        vec3 pos{};
    };

    struct BufferStructure {
        oniGLuint index{0};
        oniGLint componentCount{0};
        oniGLenum componentType{0};
        oniGLboolean normalized{0};
        oniGLsizei stride{0};
        const oniGLvoid *offset{nullptr};
    };
}
