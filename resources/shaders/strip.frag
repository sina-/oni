#version 330 core

uniform sampler2D samplers[32];

in DATA
{
    vec2 uv;
    vec4 color;
    float samplerID;
} fs_in;

out vec4 fram_color;

void main() {
    if (fs_in.samplerID >= 0.f){
        int sid = int(fs_in.samplerID + 0.1);
        fram_color = texture(samplers[sid], fs_in.uv);
    }
    else {
        fram_color = fs_in.color;
    }
}
