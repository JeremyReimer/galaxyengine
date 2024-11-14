#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(set = 0, location = 0) in vec3 fragColor;
layout(set = 0, location = 1) in vec2 fragTexCoord;

layout(set = 0, location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, fragTexCoord);
    if (outColor.w < 0.8) {
        discard;
    }
}
