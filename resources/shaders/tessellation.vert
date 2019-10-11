#version 330 core

layout (location = 0) in int samplerID;
layout (location = 1) in float ornt;
layout (location = 2) in float effect;
layout (location = 3) in vec2 halfSize;
layout (location = 4) in vec3 position;
layout (location = 5) in vec4 color;
layout (location = 6) in vec2 uv_0;
layout (location = 7) in vec2 uv_1;
layout (location = 8) in vec2 uv_2;
layout (location = 9) in vec2 uv_3;
layout (location = 10) in float transformType;

out _
{
    float samplerID;
    float ornt;
    float effect;
    vec2 halfSize;
    vec4 color;
    vec2 uv_0;
    vec2 uv_1;
    vec2 uv_2;
    vec2 uv_3;
    float transformType;
} vert;

void main()
{
    vert.color = color;
    vert.samplerID = float(samplerID);
    vert.halfSize = halfSize;
    vert.uv_0 = uv_0;
    vert.uv_1 = uv_1;
    vert.uv_2 = uv_2;
    vert.uv_3 = uv_3;
    vert.ornt = ornt;
    vert.effect = effect;
    vert.transformType = transformType;

    gl_Position = vec4(position.x, position.y, position.z, 1.0);
}
