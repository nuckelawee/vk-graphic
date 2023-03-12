#version 450

layout(binding = 0) uniform Camera {
    mat4 model;
    mat4 view;
    mat4 projection;
} camera;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texPos;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexPos;

void main() {
    gl_Position = camera.projection * camera.view * camera.model
        * vec4(position, 1.0f);

    fragColor = color;
    fragTexPos = texPos;
}
