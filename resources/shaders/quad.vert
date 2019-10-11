#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 uv;
layout (location = 3) in float samplerFront;
layout (location = 4) in float samplerBack;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out _
{
    vec4 color;
    float samplerFront;
    float samplerBack;
    vec2 uv;
} vert;

void main() {
    vert.color = color;
    vert.samplerFront = samplerFront;
    vert.samplerBack = samplerBack;
    vert.uv = uv;

    gl_Position = proj * view * model * vec4(position.x, position.y, position.z, 1.f);
}
