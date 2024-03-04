#version 460 core

// Vertex Attributes
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 normalVector;
layout(location = 2) in vec2 textureCoords;

// Uniform Variables
uniform mat4 pvm;

// Output
out vec3 normalVec;
out vec2 texCords;

// Main
void main(void) {
    // Calculate and Set Final Vertex Position
    gl_Position = pvm * vec4(vertexPosition, 1.0f);

    // Send Normal Vector and Texture Coordinates to Fragment Shader
    normalVec = normalVector;
    texCords = textureCoords;
}
