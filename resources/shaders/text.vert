#version 450 core

layout (location = 0) in vec4 position;
layout (location = 1) in int samplerID;
layout (location = 2) in vec2 uv;

uniform mat4 mvp;

out _
{
    float samplerID;
    vec2 uv;
} vert;

void main()
{
	gl_Position = mvp * position;

    // Yeah for some fucking reason int is not valid as in/out DATA: Using it
    // will cause "<program> has not been linked, or is not a program object." error upon accessing the variable.
	vert.samplerID = float(samplerID);
	vert.uv = uv;
}
