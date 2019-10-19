#version 330 core

uniform sampler2D samplers[32];

in _
{
    vec4 color;
    float samplerID;
    vec2 uv;
    float effect;
} geom;

out vec4 frag;

void mapOutputToTexture() {
    int sid = int(geom.samplerID + 0.1);
    frag = texture(samplers[sid], geom.uv);
}

void main() {
    int effect = int(geom.effect + 0.1f);

    // Default color, id = 0;
    if (effect == 0) {
        frag = geom.color;
    }
    // Default texture, id = 1
    else if (effect == 1) {
        mapOutputToTexture();
        // TODO: I could still use the color to apply tinting on texture!
    }
    // Fade, id = 2
    else if (effect == 2) {
        mapOutputToTexture();

        frag.r *= geom.color.a;
        frag.g *= geom.color.a;
        frag.b *= geom.color.a;
        frag.a *= geom.color.a;
    }
    // Tint, id = 3
    else if (effect == 3) {
        mapOutputToTexture();

        frag.r *= geom.color.r;
        frag.g *= geom.color.g;
        frag.b *= geom.color.b;
        frag.a *= geom.color.a;
    }
    else {
        frag = vec4(1, 0, 0, 1);
    }
}
