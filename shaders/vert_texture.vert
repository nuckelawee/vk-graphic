#version 450

layout(binding = 0) uniform Camera {
    mat4 model;
    mat4 view;
    mat4 projection;
} camera;
/*
layout(binding = 1) uniform Orientation {
    mat4 position;
    mat4 rotation;
    mat4 scale;
} orientation;
*/
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texPos;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexPos;

void main() {
    gl_Position = camera.projection * camera.view * camera.model
        * vec4(position.x, position.y, position.z, 1.0f);

    const vec3 lightDirection = normalize(vec3(3.0f, -5.0f, 2.0f));

    float lightIntensity = max(0.0f, dot(lightDirection, normal));

    fragColor = color * lightIntensity;
    fragTexPos = texPos;
}
