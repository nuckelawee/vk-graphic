#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexPos;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    //outColor = texture(texSampler, fragTexPos);
    outColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
}
