// Standard Headers
#include<iostream>
#include<vector>
#include<cstdlib>

// Custom Engine Headers
#define STB_IMAGE_IMPLEMENTATION
#include"Engine/Engine.hpp"

// Function Forward Declerations
static void ErrorCallback(int errorCode, const char* errorDesc);
static void FramebufferSizeCallback(GLFWwindow*, int newWinWidth, int newWinHeight);
static void MouseCallback(GLFWwindow*, double mousePosX, double mousePosY);
static void ScrollCallback(GLFWwindow*, double, double scrollPosY);
static void processKeyboardInput(GLFWwindow* win);

// Variables
// General Config
const bool vsyncEnabled {true}, msaaEnabled {true};
const unsigned int msaaSamples {4};

// Main Window Config
const std::string windowTitle {"LearningOpenGL"};
int windowWidth {800}, windowHeight {600};
float windowAspectRatio {(float)windowWidth / (float)windowHeight};
const bool windowFullscreen {true};
const float winBgColor[4] {0.2f, 0.3f, 0.3f, 1.0f};

// Delta Time
float lastTime, currentTime, deltaTime;

// Camera
sgl::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), 2.5f, 0.3f, 70.0f, 0.1f, 120.0f);

// Main
int main(void) {
    // Set GLFW Error Callback
    glfwSetErrorCallback(ErrorCallback);

    // Initialize GLFW
    if(!glfwInit()) return EXIT_FAILURE;

    // Set Window Hints
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    // Create Main Window
    GLFWwindow* win {nullptr};
    if(windowFullscreen) {
        // Get Current Physical Monitor Dimensions
        windowWidth = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
        windowHeight = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;

        // Recalculate Window Aspect Ratio
        windowAspectRatio = (float)windowWidth / (float)windowHeight;

        // Create Main Window - Fullscreen and Borderless
        win = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), glfwGetPrimaryMonitor(), nullptr);
    } else {
        // Create Main Window - Windowed and Bordered
        win = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), nullptr, nullptr);
    }

    // Check if Main Window Was Created Successfully
    if(win == nullptr) {
        // Terminate GLFW and Return Failure Exit Code
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Set Current OpenGL Render Context
    glfwMakeContextCurrent(win);

    // Initialize GLAD OpenGL 4.6 Loader
    if(!gladLoadGL(glfwGetProcAddress)) {
        // Display Error Message
        std::cerr << "Failed to Initialize GLAD OpenGL 4.6 Loader!" << std::endl;

        // Destroy Main Window and Terminate GLFW
        glfwDestroyWindow(win);
        glfwTerminate();

        // Return Failure Exit Code
        return EXIT_FAILURE;
    }

    // Enable/Disable Vertical Synchronization
    vsyncEnabled ? glfwSwapInterval(1) : glfwSwapInterval(0);

    // Set GLFW Input Mode
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set GLFW Callbacks
    glfwSetFramebufferSizeCallback(win, FramebufferSizeCallback);
    glfwSetCursorPosCallback(win, MouseCallback);
    glfwSetScrollCallback(win, ScrollCallback);

    // Enable/Disable OpenGL Functionality
    msaaEnabled ? glEnable(GL_MULTISAMPLE) : glDisable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Quad Data
    const std::vector<GLfloat> quadVertices {
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };
    const std::vector<GLuint> quadIndices {
        0, 2, 1,
        2, 3, 1
    };

    // Quad Object
    GLuint quadObj, quadVBO, quadIBO;
    {
        // Vertex Array Object
        glGenVertexArrays(1, &quadObj);
        glBindVertexArray(quadObj);

        // Vertex Buffer Object
        glGenBuffers(1, &quadVBO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(GLfloat), &quadVertices[0], GL_STATIC_DRAW);

        // Index/Element Buffer Object
        glGenBuffers(1, &quadIBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndices.size() * sizeof(GLuint), &quadIndices[0], GL_STATIC_DRAW);

        // Vertex Attributes
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)(2 * sizeof(GL_FLOAT)));
        glEnableVertexAttribArray(1);

        // Unbind Vertex Array Object
        glBindVertexArray(0);
    }

    // Models
    sgl::Model cubeModel("../assets/models/cube.obj", aiProcess_FlipUVs | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);
    sgl::Model testObj("../assets/models/survival_backpack/backpack.obj", aiProcess_Triangulate | aiProcess_FlipUVs);
    
    // Shaders
    // Post Processing Shaders
    sgl::Shader postProcessingShaders;
    postProcessingShaders.attachShader("../src/shaders/postProcessingShaders/vertexShader.glsl", sgl::ShaderType::VertexShader);
    postProcessingShaders.attachShader("../src/shaders/postProcessingShaders/fragmentShader.glsl", sgl::ShaderType::FragmentShader);
    postProcessingShaders.link();

    // Skybox Shaders
    sgl::Shader skyboxShaders;
    skyboxShaders.attachShader("../src/shaders/skyboxShaders/vertexShader.glsl", sgl::ShaderType::VertexShader);
    skyboxShaders.attachShader("../src/shaders/skyboxShaders/fragmentShader.glsl", sgl::ShaderType::FragmentShader);
    skyboxShaders.link();

    // Test Shaders
    sgl::Shader testShaders;
    testShaders.attachShader("../src/shaders/testShaders/vertexShader.glsl", sgl::ShaderType::VertexShader);
    testShaders.attachShader("../src/shaders/testShaders/fragmentShader.glsl", sgl::ShaderType::FragmentShader);
    testShaders.link();

    // Textures
    // Skybox Textures
    std::vector<std::string> skyboxTexImages {
        "../assets/textures/skybox/right.jpg",
        "../assets/textures/skybox/left.jpg",
        "../assets/textures/skybox/top.jpg",
        "../assets/textures/skybox/bottom.jpg",
        "../assets/textures/skybox/front.jpg",
        "../assets/textures/skybox/back.jpg",
    };
    sgl::Texture skyboxTexture;
    skyboxTexture.createCubemap(skyboxTexImages);
    skyboxTexture.bind();
    skyboxTexture.setParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    skyboxTexture.setParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    skyboxTexture.setParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    skyboxTexture.setParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    skyboxTexture.setParameteri(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    skyboxTexture.unbind();

    // Framebuffers
    // MSAA Framebuffer
    sgl::Framebuffer msaaFb(windowWidth, windowHeight, true, msaaSamples);
    msaaFb.createColorBufferTex();
    msaaFb.createRenderbuffer(GL_DEPTH24_STENCIL8);

    // Post-Processing Framebuffer
    sgl::Framebuffer postProcessFb(windowWidth, windowHeight, false, 0);
    postProcessFb.createColorBufferTex();

    // Model and Projection Matrix
    glm::mat4 modelMatrix, projectionMatrix;

    // Main Loop
    while(!glfwWindowShouldClose(win)) {
        // Swap Buffers
        glfwSwapBuffers(win);

        // Bind MSAA Framebuffer
        msaaFb.bind(GL_FRAMEBUFFER);

        // Clear Post Processing Framebuffer
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Projection Matrix
        projectionMatrix = glm::mat4(1.0f);
        projectionMatrix = glm::perspective(glm::radians(camera.getFOV()), windowAspectRatio, 0.1f, 100.0f);

        // Render Test Object
        {
            // Prepare to Render Test Object
            testShaders.use();

            // Send Product of Projection, View and Model Matrices (In Respective Order) to Test Object's Vertex Shader
            modelMatrix = glm::mat4(1.0f);
            testShaders.setMat4("pvm", projectionMatrix * camera.getViewMat() * modelMatrix);

            // Render Test Object
            testObj.render(testShaders);
        }

        // Render Skybox
        {
            // Prepare to Render Skybox
            glDepthFunc(GL_LEQUAL);
            skyboxShaders.use();

            // Set Skybox Cubemap Texture
            skyboxShaders.setInt("skyboxTexture", 0);
            glActiveTexture(GL_TEXTURE0);
            skyboxTexture.bind();

            // Send Product of Just The Projection and View Matrices (In Respective Order) to Skybox Object's Vertex Shader
            skyboxShaders.setMat4("pv", projectionMatrix * glm::mat4(glm::mat3(camera.getViewMat())));

            // Render Skybox
            cubeModel.render(skyboxShaders);
            glDepthFunc(GL_LESS);
        }

        // Copy Everything From MSAA Framebuffer to Post Processing Framebuffer (Downscale)
        msaaFb.bind(GL_READ_FRAMEBUFFER);
        postProcessFb.bind(GL_DRAW_FRAMEBUFFER);
        glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        // Render Quad Object
        {
            // Bind Back to Default Framebuffer and Clear it
            postProcessFb.unbind();
            glDisable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT);

            // Prepare to Render Quad Object
            glBindVertexArray(quadObj);
            postProcessingShaders.use();

            // Bind Color Buffer Texture
            postProcessingShaders.setInt("screenTexture", 0);
            glActiveTexture(GL_TEXTURE0);
            postProcessFb.bindColorBufferTex();

            // Render Quad Object
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            // Unbind Quad Object VAO
            glBindVertexArray(0);
        }

        // Calculate Delta Time
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Process User Keyboard Input
        processKeyboardInput(win);

        // Poll Events
        glfwPollEvents();
    }

    // Terminate Program
    // Destroy Vertex Array Objects
    glDeleteVertexArrays(1, &quadObj);

    // Destroy Vertex Buffer Objects
    glDeleteBuffers(1, &quadVBO);

    // Destroy Index/Element Buffer Objects
    glDeleteBuffers(1, &quadIBO);

    // Destroy Models
    cubeModel.destroy();
    testObj.destroy();

    // Destroy Shaders
    postProcessingShaders.destroy();
    testShaders.destroy();
    skyboxShaders.destroy();

    // Destroy Textures
    skyboxTexture.destroy();

    // Destroy Framebuffers
    msaaFb.destroy(true);
    postProcessFb.destroy(false);

    // Destroy Main Window and Terminate GLFW
    glfwDestroyWindow(win);
    glfwSetWindowUserPointer(win, nullptr);
    glfwTerminate();

    // Return Successful Exit Code
    return EXIT_SUCCESS;
}

// Process User Keyboard Input
static void processKeyboardInput(GLFWwindow* win) {
    // Escape Key Pressed - Terminate Program
    if(glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(win, GLFW_TRUE);
    }

    // Camera Process Keyboard Input - Move Around
    camera.processKeyboard(win, deltaTime);
}

// GLFW Callbacks
static void ErrorCallback(int errorCode, const char* errorDesc) {
    // Display GLFW Error Code and Description
    std::cerr << "GLFW Error " << errorCode << ": " << errorDesc << std::endl;
}
static void FramebufferSizeCallback(GLFWwindow*, int newWinWidth, int newWinHeight) {
    // Resize OpenGL Render Viewport
    glViewport(0, 0, newWinWidth, newWinHeight);

    // Update Window Dimensions and Recalculate Window Aspect Ratio
    windowWidth = newWinWidth;
    windowHeight = newWinHeight;
    windowAspectRatio = (float)windowWidth / (float)windowHeight;
}
static void MouseCallback(GLFWwindow*, double mousePosX, double mousePosY) {
    // Camera Process Mouse Movement - Look Around
    camera.processMouse(mousePosX, mousePosY);
}
static void ScrollCallback(GLFWwindow*, double, double scrollPosY) {
    // Camera Process Scroll - Zoom In/Out
    camera.processScroll(scrollPosY);
}
