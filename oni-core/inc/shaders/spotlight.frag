#version 330 core

layout (location = 0) out vec4 color;
uniform vec2 light_pos;

in DATA
{
    vec4 position;
    vec4 color;
} fs_in;

void main()
{
    float intensity = 1.0 / length(fs_in.position.xy - light_pos);
	color = vec4(fs_in.color.r, fs_in.color.b, fs_in.color.g,  intensity) * intensity;
}