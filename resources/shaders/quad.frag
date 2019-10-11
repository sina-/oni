#version 330 core

// TODO: This could probably be merged with all the other fragment shaders as they are all identical so far.
uniform sampler2D samplers[32];

layout(pixel_center_integer) in vec4 gl_FragCoord;

in _
{
    vec4 color;
    float samplerFront;
    float samplerBack;
    vec2 uv;
} vert;

out vec4 frag;

float clip(float n, float lower, float upper) {
    return max(lower, min(n, upper));
}

void main()
{
    /// Render texture, blend it with another texture, output to target
    if (vert.samplerFront >= -0.1f && vert.samplerBack >= -0.1f) {
        ivec2 screenPos = ivec2(gl_FragCoord.xy);
        int sidBack = int(vert.samplerBack + 0.1);
        vec4 old = texelFetch(samplers[sidBack], screenPos, 0);

        int sid = int(vert.samplerFront + 0.1);
        vec4 new = texture(samplers[sid], vert.uv);

        frag.r = mix(old.r, new.r, new.a);
        frag.g = mix(old.g, new.g, new.a);
        frag.b = mix(old.b, new.b, new.a);
        frag.a = clip(old.a + new.a, 0, 1);

        // NOTE: Always pre-multiplied alpha!
        // TODO: Don't know if this is needed.
        frag.r *= frag.a;
        frag.g *= frag.a;
        frag.b *= frag.a;
    }
    /// Normal texture rendering
    else if (vert.samplerFront >= -0.1f){
        int sid = int(vert.samplerFront + 0.1);
        frag = texture(samplers[sid], vert.uv);
    }
    // Render color, blend it with another texture, output to target
    else if (vert.samplerBack >= -0.1f){
        ivec2 screenPos = ivec2(gl_FragCoord.xy);
        int sidBack = int(vert.samplerBack + 0.1);

        vec4 old = texelFetch(samplers[sidBack], screenPos, 0);

        frag.r = mix(old.r, vert.color.r, vert.color.a);
        frag.g = mix(old.g, vert.color.g, vert.color.a);
        frag.b = mix(old.b, vert.color.b, vert.color.a);
        frag.a = clip(old.a + vert.color.a, 0, 1);

        // NOTE: Always pre-multiplied alpha!
        frag.r *= vert.color.a;
        frag.g *= vert.color.a;
        frag.b *= vert.color.a;
    }
    // Normal color rendering
    else {
        frag = vert.color;
    }
}
