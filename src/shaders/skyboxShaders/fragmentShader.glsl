#version 460 core

// Input
in vec3 texCords;

// Uniform Variables
uniform samplerCube skyboxTexture;

// Output
out vec4 fragColor;

// Main
void main(void) {
    // Calculate and Set Final Fragment Color
    fragColor = texture(skyboxTexture, texCords);
}
