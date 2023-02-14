#version 450

layout(binding = 0) uniform Camera {
    mat4 model;
    mat4 view;
    mat4 projection;
} camera;

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = camera.projection * camera.view * camera.model
        * vec4(position, 0.0f, 1.0f);
    fragColor = color;
}
