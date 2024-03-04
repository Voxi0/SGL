#version 460 core

// Structures
// Material
struct Material {
    sampler2D texture_diffuse1, texture_specular1;
    float shininess;
};

// Light Casters
struct DirectionalLight {
    vec3 direction;
    vec3 ambientIntensity, diffusionIntensity, specularIntensity;
};
struct PointLight {
    vec3 position;
    vec3 ambientIntensity, diffusionIntensity, specularIntensity;
    float attenuationConstant, attenuationLinear, attenuationQuadratic;
};
struct SpotLight {
    vec3 position, direction;
    float innerCutOff, outerCutOff;
    float attenuationConstant, attenuationLinear, attenuationQuadratic;
};

// Function Forward Declerations
vec3 calculateDirectionalLight();
vec3 calculatePointLight();
vec3 calculateSpotLight();

// Input
in vec3 normalVec;
in vec2 texCords;

// Uniform Variables
uniform Material material;
uniform SpotLight light;

// Output
out vec4 fragColor;

// Main
void main(void) {
    // Calculate and Set Final Fragment Color
    fragColor = texture(material.texture_diffuse1, texCords);
}
