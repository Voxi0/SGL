// Standard Headers
#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>

// GLAD OpenGL 4.6 Loader
#include<glad/gl.h>

// GLFW
#define GLFW_INCLUDE_NONE
#include<GLFW/glfw3.h>

// OpenGL Mathematics - GLM
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

// STB
#include<stb_image.h>

// Assimp
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

// Namespace
namespace sgl {
    // Shader
    enum ShaderType {VertexShader, FragmentShader, GeometryShader};
    class Shader {
        public:
            // Attach Shader
            void attachShader(const std::string &shaderFilePath, const sgl::ShaderType &shaderType);

            // Link Shader Program and Check For Link Errors
            void link(void);

            // Use/Destroy Shaders
            void use(void);
            void destroy(void);

            // Setters - Set Uniform Variables
            void setBool(const std::string &uniformVarName, const bool &value);
            void setInt(const std::string &uniformVarName, const int &value);
            void setFloat(const std::string &uniformVarName, const float &value);
            void setDouble(const std::string &uniformVarName, const double &value);

            void setVec2(const std::string &uniformVarName, const glm::vec2 &value);
            void setVec2(const std::string &uniformVarName, const float &a, const float &b);

            void setVec3(const std::string &uniformVarName, const glm::vec3 &value);
            void setVec3(const std::string &uniformVarName, const float &a, const float &b, const float &c);

            void setVec4(const std::string &uniformVarName, const glm::vec4 &value);
            void setVec4(const std::string &uniformVarName, const float &a, const float &b, const float &c, const float &d);

            void setMat4(const std::string &uniformVarName, const glm::mat4 &value);
        private:
            // Shaders - Shader Program
            GLuint shaderProgram {glCreateProgram()};
    };

    // Texture
    class Texture {
        public:
            // Create 2D Texture Image
            void create2DImg(const std::string &texImgFilePath);

            // Create A Cubemap Texture Object
            void createCubemap(const std::vector<std::string> &texImgFilePaths);

            // Set Texture Parameter
            void setParameteri(const GLenum &parameter, const GLint &value);
            void setParameterf(const GLenum &parameter, const GLfloat &value);

            // Bind/Unbind/Destroy Texture Object
            void bind(void);
            void unbind(void);
            void destroy(void);
        private:
            // Texture Object
            GLuint texture;
            GLenum bindTarget;
    };

    // Framebuffer
    class Framebuffer {
        public:
            // Constructor - Create A Framebuffer
            Framebuffer(const unsigned int &windowWidth, const unsigned int &windowHeight, const bool &msaaEnabled, const unsigned int &msaaSamples);

            // Create Color Buffer Texture
            void createColorBufferTex(void);

            // Create Renderbuffer (Depth and Stencil Buffer) Object / Depth and Stencil Buffer Texture
            void createRenderbuffer(const GLenum &format);
            void createDepthStencilTex(const GLenum &format);

            // Binders
            void bind(const GLenum &bindTarget);
            void bindColorBufferTex(void);

            // Unbind Framebuffer Object
            void unbind(void);

            // Destroy Framebuffer Object
            void destroy(const bool &usingRenderbuffer);
        private:
            // Window Dimensions
            unsigned int windowWidth, windowHeight;

            // MSAA
            bool msaaEnabled;
            unsigned int msaaSamples;

            // Framebuffer Object
            GLuint framebuffer;

            // Color Buffer Texture
            GLuint colorBufferTex;

            // Renderbuffer Object - Depth and Stencil Buffer
            GLuint renderBuffer;

            // Depth and Stencil Buffer Texture - Alternative to Renderbuffer
            GLuint depthStencilTex;
    };

    // Model
    struct VertexStruct {
        glm::vec3 position, normalVector;
        glm::vec2 textureCoords;
    };
    struct TextureStruct {
        GLuint id;
        std::string type, path;
    };
    class Mesh {
        public:
            // Mesh Data
            std::vector<VertexStruct> vertices;
            std::vector<GLuint> indices;
            std::vector<TextureStruct> textures;

            // Constructor - Create A Mesh Object
            Mesh(const std::vector<VertexStruct> &vertices, const std::vector<GLuint> &indices, const std::vector<TextureStruct> &textures);

            // Render Mesh Object
            void render(sgl::Shader &shader);

            // Destroy Mesh
            void destroy(void);
        private:
            // Render Data
            GLuint VAO, VBO, IBO;
    };
    class Model {
        public:
            // Constructor - Load Model File
            Model(const std::string &modelFilePath, const unsigned int &processFlags);

            // Render Model
            void render(sgl::Shader &shader);

            // Destroy Model - Destroys All The Meshes
            void destroy(void);
        private:
            // Full Path to Model File
            std::string directory;

            // Meshes
            std::vector<sgl::Mesh> meshes;

            // Loaded Textures
            std::vector<sgl::TextureStruct> loadedTextures;

            // Process All The Nodes/Meshes in The Model
            void processNode(aiNode* node, const aiScene* scene);
            sgl::Mesh processMesh(aiMesh* mesh, const aiScene* scene);

            // Process All The Model's Materials/Textures
            std::vector<sgl::TextureStruct> loadMaterialTextures(aiMaterial* material, aiTextureType texType, const std::string &texTypeName);
    };

    // Camera
    class Camera {
        public:
            // Constructor - Initialize The Camera
            Camera(const glm::vec3 &position, const glm::vec3 &front, const float &moveSpeed, const float &lookSpeed, const float &fov, const float &minFOV, const float &maxFOV);

            // Process Keyboard Input - Move Around
            void processKeyboard(GLFWwindow* win, const float &deltaTime);

            // Process Mouse Movement (Look Around) and Scroll (Zoom In/Out)
            void processMouse(const double &mousePosX, const double &mousePosY);
            void processScroll(const double &scrollPosY);

            // Getters
            glm::mat4 getViewMat(void);
            float getFOV(void);

            // Setters
            void setFirstMouse(const bool &value);
        private:
            glm::vec3 position, front, up {0.0f, 1.0f, 0.0f};
            float moveSpeed, lookSpeed;
            float fov, minFOV, maxFOV;
            bool firstMouse {true};
            float lastX, lastY;
            float yaw {-90.0f}, pitch;
    };
};
