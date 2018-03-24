#version 450 core

layout (location = 0) out vec4 color;
uniform sampler2D samplers[32];

in DATA
{
    float samplerID;
    vec2 uv;
} fs_in;

void main()
{
    // Compensate for epsilone of error.
    int sid = int(fs_in.samplerID + 0.1);
	color = texture(samplers[sid], fs_in.uv);
}
