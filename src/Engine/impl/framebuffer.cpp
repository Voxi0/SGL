#include"../Engine.hpp"

// Constructor - Create A Framebuffer
sgl::Framebuffer::Framebuffer(const unsigned int &windowWidth, const unsigned int &windowHeight, const bool &msaaEnabled, const unsigned int &msaaSamples) {
    // Set Window Width and Height
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;

    // MSAA
    this->msaaEnabled = msaaEnabled;
    this->msaaSamples = msaaSamples;

    // Create A Framebuffer Object
    glGenFramebuffers(1, &this->framebuffer);
}

// Create Color Buffer Texture
void sgl::Framebuffer::createColorBufferTex(void) {
    // Bind Framebuffer Object
    glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);

    // Create A Color Buffer Texture
    glGenTextures(1, &this->colorBufferTex);
    if(this->msaaEnabled) {
        // Create Antialiased Color Buffer Texture
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->colorBufferTex);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, this->msaaSamples, GL_RGB, this->windowWidth, this->windowHeight, GL_TRUE);

        // Texture Parameters
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Attach Color Buffer Texture to Framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, this->colorBufferTex, 0);

        // Unbind Color Buffer Texture
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    } else {
        // Create A Non-Antialiased Color Buffer Texture
        glBindTexture(GL_TEXTURE_2D, this->colorBufferTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->windowWidth, this->windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        // Texture Parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Attach Color Buffer Texture to Framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colorBufferTex, 0);

        // Unbind Color Buffer Texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Unbind Framebuffer Object
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Create Renderbuffer Object
void sgl::Framebuffer::createRenderbuffer(const GLenum &format) {
    // Bind Framebuffer Object
    glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);

    // Create A Renderbuffer Object
    glGenRenderbuffers(1, &this->renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, this->renderBuffer);
    if(this->msaaEnabled) {
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, this->msaaSamples, format, this->windowWidth, this->windowHeight);
    } else {
        glRenderbufferStorage(GL_RENDERBUFFER, format, this->windowWidth, this->windowHeight);
    }

    // Attach Renderbuffer Object to Framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->renderBuffer);

    // Check if Framebuffer Object is Complete
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        // Display Error Message and Destroy Framebuffer Object
        std::cerr << "Failed to Create Framebuffer Object!" << std::endl;
        glDeleteFramebuffers(1, &this->framebuffer);
    }

    // Unbind Renderbuffer and Framebuffer Object
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void sgl::Framebuffer::createDepthStencilTex(const GLenum &format) {
    // Bind Framebuffer Object
    glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);

    // Create A Depth and Stencil Buffer Texture
    glGenTextures(1, &this->depthStencilTex);
    if(this->msaaEnabled) {
        // Create Antialiased Depth and Stencil Buffer Texture
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->depthStencilTex);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, this->msaaSamples, format, this->windowWidth, this->windowHeight, GL_TRUE);

        // Attach Depth and Stencil Buffer Texture to Framebuffer Object
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, this->depthStencilTex, 0);

        // Unbind Depth and Stencil Buffer Texture
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    } else {
        // Create Non-Antialiased Depth and Stencil Buffer Texture
        glBindTexture(GL_TEXTURE_2D, this->depthStencilTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->windowWidth, this->windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        // Attach Depth and Stencil Buffer Texture to Framebuffer Object
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, this->depthStencilTex, 0);

        // Unbind Depth and Stencil Buffer Texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Check if Framebuffer Object is Complete
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        // Display Error Message and Destroy Framebuffer Object
        std::cerr << "Failed to Create Framebuffer Object!" << std::endl;
        glDeleteFramebuffers(1, &this->framebuffer);
    }

    // Unbind Framebuffer Object
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Binders
void sgl::Framebuffer::bind(const GLenum &bindTarget) {
    if(bindTarget == GL_FRAMEBUFFER || bindTarget == GL_READ_FRAMEBUFFER || bindTarget == GL_DRAW_FRAMEBUFFER) {
        glBindFramebuffer(bindTarget, this->framebuffer);
    } else {
        // Display Error Message
        std::cerr << "Invalid Framebuffer Bind Target!" << std::endl;
    }
}
void sgl::Framebuffer::bindColorBufferTex(void) {
    if(this->msaaEnabled) {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->colorBufferTex);
    } else {
        glBindTexture(GL_TEXTURE_2D, this->colorBufferTex);
    }
}

// Unbind Framebuffer Object
void sgl::Framebuffer::unbind(void) {glBindFramebuffer(GL_FRAMEBUFFER, 0);}

// Destroy Framebuffer Object
void sgl::Framebuffer::destroy(const bool &usingRenderbuffer) {
    // Delete Framebuffer Object
    glDeleteFramebuffers(1, &this->framebuffer);

    // Delete Color Buffer Texture
    glDeleteTextures(1, &this->colorBufferTex);

    // Delete Depth and Stencil Buffer
    if(usingRenderbuffer) {
        glDeleteRenderbuffers(1, &this->renderBuffer);
    } else {
        glDeleteTextures(1, &this->depthStencilTex);
    }
}
