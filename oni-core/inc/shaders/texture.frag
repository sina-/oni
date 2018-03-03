#version 450 core

layout (location = 0) out vec4 color;
uniform sampler2D textureSamplers[32];

in DATA
{
    float tid;
    vec2 uv;
} fs_in;

void main()
{
    // Yeah for some fucking reason int is not valid as in/out DATA: Using it
    // will cause "<program> has not been linked, or is not a program object." error upon accessing the variable.
    // Have to use float and this awkard cast to compensate for epsilone.
    int tid = int(fs_in.tid + 0.1);
	color = texture(textureSamplers[tid], fs_in.uv);
}
