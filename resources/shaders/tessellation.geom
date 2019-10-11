#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 proj;
uniform mat4 view;

in _ {
    float samplerID;
    float ornt;
    float effect;
    vec2 halfSize;
    vec4 color;
    vec2 uv_0;
    vec2 uv_1;
    vec2 uv_2;
    vec2 uv_3;
    float transformType;
} vert[];

out _ {
    vec4 color;
    float samplerID;
    vec2 uv;
    float effect;
} geom;

mat4 identity = mat4(
1, 0, 0, 0,
0, 1, 0, 0,
0, 0, 1, 0,
0, 0, 0, 1
);

mat4 rotation(float ornt) {
    float c = cos(ornt);
    float s = sin(ornt);
    float omc = 1.0f - c;

    float x = 0;
    float y = 0;
    float z = 1;

    return mat4(
    x * omc + c, y * x * omc + z * s, x * y * omc - y * s, 0,
    x * y * omc - z * s, y * omc + c, y * z * omc + x * s, 0,
    x * z * omc + y * s, y * z * omc - x * s, z * omc + c, 0,
    0, 0, 0, 1);
}

mat4 scale(float x, float y, float z) {
    mat4 result = mat4(
    x, 0, 0, 0,
    0, y, 0, 0,
    0, 0, z, 0,
    0, 0, 0, 1
    );

    return result;
}

mat4 translation(vec4 pos) {
    mat4 result = mat4(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    pos.x, pos.y, pos.z, 1
    );
    return result;
}

mat4 transformation(vec4 pos, float ornt) {
    mat4 trans = translation(pos);
    mat4 rota = rotation(ornt);
    // TODO: Not scaling
    //mat4 scale = scale(1, 1, 1);
    mat4 result = trans * rota;// * scale;
    return result;
}

vec3 multiply(mat4 mat, vec3 vec) {
    vec3 result = vec3(
    mat[0][0] * vec.x + mat[1][0] * vec.y + mat[2][0] * vec.z + mat[3][0],
    mat[0][1] * vec.x + mat[1][1] * vec.y + mat[2][1] * vec.z + mat[3][1],
    mat[0][2] * vec.x + mat[1][2] * vec.y + mat[2][2] * vec.z + mat[3][2]);
    return result;
}

vec4 mvp(vec4 model, int type) {
    if (type == 0) {
        vec4 result = proj * view * model;
        return result;
    }
    else if (type == 1) {
        vec4 result = proj * model;
        return result;
    }
    else {
        return vec4(1, 1, 1, 1);
    }
}

void tesselate(vec4 pos) {
    vec2 halfSize = vert[0].halfSize;
    float ornt = vert[0].ornt;
    geom.color = vert[0].color;// vert[0] since there's only one input vertex
    geom.samplerID = vert[0].samplerID;
    geom.effect = vert[0].effect;
    int type = int(vert[0].transformType + 0.1f);

    mat4 transformation = transformation(pos, ornt);
    // NOTE: The translation matrix has the Z value! So I pass 0 here for z.
    vec4 model = transformation * vec4(-halfSize.x, -halfSize.y, 0, 1);

    // TODO: kinda annoying that the vertex emit order is different than uv order
    gl_Position = mvp(model, type);
    geom.uv = vert[0].uv_0;
    EmitVertex();

    model = transformation * vec4(halfSize.x, -halfSize.y, 0, 1);

    gl_Position = mvp(model, type);
    geom.uv = vert[0].uv_3;
    EmitVertex();

    model = transformation * vec4(-halfSize.x, halfSize.y, 0, 1);

    gl_Position = mvp(model, type);
    geom.uv = vert[0].uv_1;
    EmitVertex();

    model = transformation * vec4(halfSize.x, halfSize.y, 0, 1);

    gl_Position = mvp(model, type);
    geom.uv = vert[0].uv_2;
    EmitVertex();

    EndPrimitive();
}

void main() {
    tesselate(gl_in[0].gl_Position);
}
