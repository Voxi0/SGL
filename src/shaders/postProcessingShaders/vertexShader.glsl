#version 460 core

// Vertex Attributes
layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 textureCoords;

// Output
out vec2 texCords;

// Main
void main(void) {
    // Calculate and Set Final Vertex Position
    gl_Position = vec4(vertexPosition, 0.0f, 1.0f);

    // Send Texture Coordinates to Fragment Shader
    texCords = textureCoords;
}
