#version 450

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    //outColor = vec4(1.0, 0.0, 0.0, 1.0); // Solid red, here for debugging
    //we have to flip the x tex, for it to look correct
    outColor = texture(texSampler, vec2(1-fragTexCoord.x, fragTexCoord.y));


}