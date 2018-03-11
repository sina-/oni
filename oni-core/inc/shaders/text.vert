#version 450 core

layout (location = 0) in vec4 position;
layout (location = 2) in int samplerID;
layout (location = 3) in vec2 uv;

uniform mat4 pr_matrix;
uniform mat4 vw_matrix = mat4(1.0);
uniform mat4 ml_matrix = mat4(1.0);

out DATA
{
    float samplerID;
    vec2 uv;
} vs_out;

void main()
{
	gl_Position = pr_matrix * vw_matrix * ml_matrix * position;

    // Yeah for some fucking reason int is not valid as in/out DATA: Using it
    // will cause "<program> has not been linked, or is not a program object." error upon accessing the variable.
	vs_out.samplerID = float(samplerID);
	vs_out.uv = uv;
}
