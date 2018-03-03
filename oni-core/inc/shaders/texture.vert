#version 450 core

// NOTE: This has to map to indices defined in batch-renderer-2d.cpp
// TODO: Can this be set automatically by using the same indices as renderer?
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout (location = 2) in int tid;
layout (location = 3) in vec2 uv;

uniform mat4 pr_matrix;
uniform mat4 vw_matrix = mat4(1.0);
uniform mat4 ml_matrix = mat4(1.0);

out DATA
{
    float tid;
    vec2 uv;
} vs_out;

void main()
{
	gl_Position = pr_matrix * vw_matrix * ml_matrix * position;
	vs_out.tid = float(tid);
	vs_out.uv = uv;
}
