#include"../Engine.hpp"

// Create A Shader From It's Source File
void sgl::Shader::attachShader(const std::string &shaderFilePath, const sgl::ShaderType &shaderType) {
    // Shader File
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);

    // Shader C++ Source File
    std::string shaderCppSrc;

    // Try to Open and Read Shader File
    try {
        shaderFile.open(shaderFilePath, std::ifstream::in | std::ifstream::binary);

        std::stringstream buffer;
        buffer << shaderFile.rdbuf();
        shaderCppSrc = buffer.str();

        shaderFile.close();
    } catch(std::ifstream::failure &e) {
        // Display Error Message
        std::cerr << "Failed to Read Shader File! " << shaderFilePath << std::endl;
    }

    // Convert Shader Source in C++ String to C String
    const char* shaderSrc {shaderCppSrc.c_str()};

    // Shader
    GLuint shader;
    switch(shaderType) {
        case sgl::ShaderType::VertexShader:
            shader = glCreateShader(GL_VERTEX_SHADER);
            break;
        case sgl::ShaderType::FragmentShader:
            shader = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        case sgl::ShaderType::GeometryShader:
            shader = glCreateShader(GL_GEOMETRY_SHADER);
            break;
        default:
            // Display Error Message
            std::cerr << "Unknown Shader Type Specified!" << std::endl;
            break;
    }
    glShaderSource(shader, 1, &shaderSrc, nullptr);
    glCompileShader(shader);

    // Check For Shader Compilation Errors
    int success;
    char shaderInfoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        // Get Shader Info Log
        glGetShaderInfoLog(shader, 1024, nullptr, shaderInfoLog);

        // Display Error Message and Shader Info Log
        std::cerr << "Shader Compilation Error!\n";
        std::cerr << "Info Log: " << shaderInfoLog << std::endl;
    } else {
        // Delete Shader After Attaching it To The Shader Program - It's No Longer Needed
        glAttachShader(this->shaderProgram, shader);
        glDeleteShader(shader);
    }
}

// Link Shader Program and Check For Link Errors
void sgl::Shader::link(void) {
    // Link Shader Program
    glLinkProgram(this->shaderProgram);

    // Check For Shader Program Link Errors
    int success;
    char infoLog[1024];
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        // Get Shader Program Info Log
        glGetProgramInfoLog(this->shaderProgram, 1024, nullptr, infoLog);

        // Display Error Message and Shader Program Info Log
        std::cerr << "Shader Program Link Error!\n";
        std::cerr << "Info Log: " << infoLog << std::endl;

        // Delete Shader Program
        glDeleteProgram(this->shaderProgram);
    }
}

// Use/Destroy Shaders
void sgl::Shader::use(void) {glUseProgram(this->shaderProgram);}
void sgl::Shader::destroy(void) {glDeleteProgram(this->shaderProgram);}

// Setters - Set Uniform Variables
void sgl::Shader::setBool(const std::string &uniformVarName, const bool &value) {
    glUniform1i(glGetUniformLocation(this->shaderProgram, uniformVarName.c_str()), value);
}
void sgl::Shader::setInt(const std::string &uniformVarName, const int &value) {
    glUniform1i(glGetUniformLocation(this->shaderProgram, uniformVarName.c_str()), value);
}
void sgl::Shader::setFloat(const std::string &uniformVarName, const float &value) {
    glUniform1f(glGetUniformLocation(this->shaderProgram, uniformVarName.c_str()), value);
}
void sgl::Shader::setDouble(const std::string &uniformVarName, const double &value) {
    glUniform1d(glGetUniformLocation(this->shaderProgram, uniformVarName.c_str()), value);
}

void sgl::Shader::setVec2(const std::string &uniformVarName, const glm::vec2 &value) {
    glUniform2fv(glGetUniformLocation(this->shaderProgram, uniformVarName.c_str()), 1, &value[0]);
}
void sgl::Shader::setVec2(const std::string &uniformVarName, const float &a, const float &b) {
    glUniform2f(glGetUniformLocation(this->shaderProgram, uniformVarName.c_str()), a, b);
}

void sgl::Shader::setVec3(const std::string &uniformVarName, const glm::vec3 &value) {
    glUniform3fv(glGetUniformLocation(this->shaderProgram, uniformVarName.c_str()), 1, &value[0]);
}
void sgl::Shader::setVec3(const std::string &uniformVarName, const float &a, const float &b, const float &c) {
    glUniform3f(glGetUniformLocation(this->shaderProgram, uniformVarName.c_str()), a, b, c);
}

void sgl::Shader::setVec4(const std::string &uniformVarName, const glm::vec4 &value) {
    glUniform4fv(glGetUniformLocation(this->shaderProgram, uniformVarName.c_str()), 1, &value[0]);
}
void sgl::Shader::setVec4(const std::string &uniformVarName, const float &a, const float &b, const float &c, const float &d) {
    glUniform4f(glGetUniformLocation(this->shaderProgram, uniformVarName.c_str()), a, b, c, d);
}

void sgl::Shader::setMat4(const std::string &uniformVarName, const glm::mat4 &value) {
    glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram, uniformVarName.c_str()), 1, GL_FALSE, &value[0][0]);
}
