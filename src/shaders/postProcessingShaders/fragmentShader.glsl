#version 460 core

// Input
in vec2 texCords;

// Uniform Variables
uniform sampler2D screenTexture;

// Output
out vec4 fragColor;

// Main
void main(void) {
    // Calculate and Set Final Fragment Color
    fragColor = texture(screenTexture, texCords);
}
