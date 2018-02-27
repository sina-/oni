#version 330 core

layout (location = 0) out vec4 color;
uniform sampler2D tex;

in DATA
{
    vec4 position;
    vec4 color;
    vec2 uv;
} fs_in;


void main()
{
    //color = fs_in.color;
	color = texture(tex, fs_in.uv);
}
