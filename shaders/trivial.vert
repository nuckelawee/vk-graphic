#version 450

vec2 pPosition2D[] = {
    vec2( 0.0, -0.5),
    vec2( 0.5,  0.5),
    vec2(-0.5,  0.5)
};

vec3 pFragColor2D[] = {
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
};

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(pPosition2D[gl_VertexIndex], 0.0, 1.0);
    fragColor = pFragColor2D[gl_VertexIndex];
}
