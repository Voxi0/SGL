#include"../Engine.hpp"
#include <GL/gl.h>

// Create 2D Texture Image
void sgl::Texture::create2DImg(const std::string &texImgFilePath) {
    // Create A Texture Object
    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    this->bindTarget = GL_TEXTURE_2D;

    // Flip Texture Images Vertically Upon Loading Them
    stbi_set_flip_vertically_on_load(true);

    // Load Texture Image File
    int texImgWidth, texImgHeight, texImgNrChannels;
    unsigned char* texImg {stbi_load(texImgFilePath.c_str(), &texImgWidth, &texImgHeight, &texImgNrChannels, 0)};
    if(texImg) {
        // Figure Out Texture Image Format
        GLenum texImgFormat;
        switch(texImgNrChannels) {
            case 3:
                texImgFormat = GL_RGB;
                break;
            case 4:
                texImgFormat = GL_RGBA;
                break;
        }

        // Load Texture Image Data Into Texture Object and Generate Mipmaps
        glTexImage2D(GL_TEXTURE_2D, 0, texImgFormat, texImgWidth, texImgHeight, 0, texImgFormat, GL_UNSIGNED_BYTE, texImg);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        // Delete Texture Object and Display Error Message
        std::cerr << "Failed to Load Texture Image File and Create Texture!" << std::endl;
        glDeleteTextures(1, &this->texture);
        return;
    }

    // Free Texture Image Data From Memory
    stbi_image_free(texImg);

    // Unbind Texture Object
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Create A Cubemap Texture Object
void sgl::Texture::createCubemap(const std::vector<std::string> &texImgFilePaths) {
    // Create A Texture Object
    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);
    this->bindTarget = GL_TEXTURE_CUBE_MAP;

    // Don't Flip Texture Images Vertically Upon Loading Them
    stbi_set_flip_vertically_on_load(false);

    // Load Texture Image Files
    int texImgWidth, texImgHeight, texImgNrChannels;
    unsigned char* texImg;
    for(unsigned int i {0}; i < texImgFilePaths.size(); i++) {
        texImg = stbi_load(texImgFilePaths[i].c_str(), &texImgWidth, &texImgHeight, &texImgNrChannels, 0);
        if(texImg) {
            // Load Texture Image Data Into Texture Object
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, texImgWidth, texImgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texImg);

            // Free Texture Image Data From Memory
            stbi_image_free(texImg);
        } else {
            // Destroy Texture Object and Display Error Message
            std::cerr << "Failed to Load Skybox Texture Image: " << texImgFilePaths[i] << "!\n";
            glDeleteTextures(1, &this->texture);

            // Free Texture Image Data From Memory
            stbi_image_free(texImg);
            return;
        }
    }

    // Unbind Texture Object
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

// Set Texture Parameter
void sgl::Texture::setParameteri(const GLenum &parameter, const GLint &value) {glTexParameteri(this->bindTarget, parameter, value);}
void sgl::Texture::setParameterf(const GLenum &parameter, const GLfloat &value) {glTexParameterf(this->bindTarget, parameter, value);}

// Bind/Unbind/Destroy Texture Object
void sgl::Texture::bind(void) {glBindTexture(this->bindTarget, this->texture);}
void sgl::Texture::unbind(void) {glBindTexture(this->bindTarget, 0);}
void sgl::Texture::destroy(void) {glDeleteTextures(1, &this->texture);}
