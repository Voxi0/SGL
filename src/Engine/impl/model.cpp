#include"../Engine.hpp"

// Constructor - Create A Mesh Object
sgl::Mesh::Mesh(const std::vector<sgl::VertexStruct> &vertices, const std::vector<GLuint> &indices, const std::vector<TextureStruct> &textures) {
    // Initialize Mesh Data
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    // Create VAO
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    // Create VBO
    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexStruct), &vertices[0], GL_STATIC_DRAW);

    // Create IBO
    glGenBuffers(1, &this->IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Vertex Attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStruct), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStruct), (void*)offsetof(VertexStruct, normalVector));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexStruct), (void*)offsetof(VertexStruct, textureCoords));
    glEnableVertexAttribArray(2);

    // Unbind VAO
    glBindVertexArray(0);
}

// Render Mesh Object
void sgl::Mesh::render(sgl::Shader &shader) {
    unsigned int diffuseNr {1}, specularNr {1};
    for(unsigned int i {0}; i < this->textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number, name {this->textures[i].type};
        if(name == "texture_diffuse") {
            number = std::to_string(diffuseNr++);
        } else if(name == "texture_specular") {
            number = std::to_string(specularNr++);
        }

        shader.setInt("material." + name + number, i);
        glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // Render Mesh
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

// Destroy Mesh
void sgl::Mesh::destroy(void) {
    // VAO, VBO and IBO
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->IBO);

    // Textures
    for(unsigned int i {0}; i < this->textures.size(); i++) {
        glDeleteTextures(1, &this->textures[i].id);
    }
}

// Constructor - Load Model File
sgl::Model::Model(const std::string &modelFilePath, const unsigned int &processFlags) {
    // Import Model File
    Assimp::Importer importer;
    const aiScene* scene {importer.ReadFile(modelFilePath, processFlags)};
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        // Display Error Message
        std::cerr << "Failed to Load/Import Model File!\n";
        std::cerr << "Error: " << importer.GetErrorString() << std::endl;
        return;
    }

    // Retrieve Full Path to Model File
    this->directory = modelFilePath.substr(0, modelFilePath.find_last_of('/'));

    // Process All The Nodes in The Model
    this->processNode(scene->mRootNode, scene);
}

// Process All The Nodes/Meshes in The Model
void sgl::Model::processNode(aiNode* node, const aiScene* scene) {
    // Process All The Node's Meshes if Any
    for(unsigned int i {0}; i < node->mNumMeshes; i++) {
        aiMesh* mesh {scene->mMeshes[node->mMeshes[i]]};
        this->meshes.push_back(this->processMesh(mesh, scene));
    }

    // Process All The Child Nodes if Any
    for(unsigned int i {0}; i < node->mNumChildren; i++) {
        this->processNode(node->mChildren[i], scene);
    }
}
sgl::Mesh sgl::Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    // Mesh Data
    std::vector<sgl::VertexStruct> vertices;
    std::vector<GLuint> indices;
    std::vector<sgl::TextureStruct> textures;

    // Process All The Mesh Vertices
    for(unsigned int i {0}; i < mesh->mNumVertices; i++) {
        sgl::VertexStruct vertex;

        // Vertex Positions
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        // Normal Vectors if Any
        if(mesh->HasNormals()) {
            vertex.normalVector.x = mesh->mNormals[i].x;
            vertex.normalVector.y = mesh->mNormals[i].y;
            vertex.normalVector.z = mesh->mNormals[i].z;
        } else {
            vertex.normalVector = glm::vec3(0.0f);
        }

        // Texture Coordinates if Any
        if(mesh->mTextureCoords[0]) {
            vertex.textureCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.textureCoords.y = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.textureCoords = glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
    }

    // Process All The Mesh Indices
    for(unsigned int i {0}; i < mesh->mNumFaces; i++) {
        aiFace face {mesh->mFaces[i]};
        for(unsigned int j {0}; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Process All The Mesh Materials/Textures if Any
    if(mesh->mMaterialIndex >= 0) {
        aiMaterial* material {scene->mMaterials[mesh->mMaterialIndex]};
        
        std::vector<sgl::TextureStruct> diffuseMaps {this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse")};
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        
        std::vector<sgl::TextureStruct> specularMaps {this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular")};
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    // Return Mesh Object
    return sgl::Mesh(vertices, indices, textures);
}

// Load A 2D Texture Image File
GLuint load2DTex(const std::string &texImgFilePath) {
    // Create A Texture Object
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Flip Texture Images Vertically Upon Loading Them
    stbi_set_flip_vertically_on_load(true);

    // Load 2D Texture Image File
    int texImgWidth, texImgHeight, texImgNrChannels;
    unsigned char* texImg {stbi_load(texImgFilePath.c_str(), &texImgWidth, &texImgHeight, &texImgNrChannels, 0)};
    if(texImg) {
        // Figure Out Texture Image Format
        GLenum texImgFormat;
        switch(texImgNrChannels) {
            case 1:
                texImgFormat = GL_RED;
                break;
            case 3:
                texImgFormat = GL_RGB;
                break;
            case 4:
                texImgFormat = GL_RGBA;
                break;
            default:
                // Display Error Message and Destroy Texture
                std::cerr << "Unknown Texture Image Format!" << std::endl;
                stbi_image_free(texImg);
                glDeleteTextures(1, &texture);
        }

        // Load Texture Image Data Into Texture Object and Generate Mipmaps
        glTexImage2D(GL_TEXTURE_2D, 0, texImgFormat, texImgWidth, texImgHeight, 0, texImgFormat, GL_UNSIGNED_BYTE, texImg);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        // Display Error Message and Destroy Texture Object
        std::cerr << "Failed to Load Texture Image File! " << texImgFilePath << std::endl;
        glDeleteTextures(1, &texture);
    }

    // Free Texture Image Data From Memory
    stbi_image_free(texImg);

    // Return Texture Object
    return texture;
}

// Process All The Model's Materials/Textures
std::vector<sgl::TextureStruct> sgl::Model::loadMaterialTextures(aiMaterial* material, aiTextureType texType, const std::string &texTypeName) {
    std::vector<sgl::TextureStruct> textures;
    for(unsigned int i {0}; i < material->GetTextureCount(texType); i++) {
        aiString str;
        material->GetTexture(texType, i, &str);
        bool skip {false};

        for(unsigned int j {0}; j < this->loadedTextures.size(); j++) {
            if(std::strcmp(this->loadedTextures[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(this->loadedTextures[j]);
                skip = true;
                break;
            }
        }

        if(!skip) {
            sgl::TextureStruct texture;
            texture.id = load2DTex(this->directory + '/' + str.C_Str());
            texture.type = texTypeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            this->loadedTextures.push_back(texture);
        }
    }
    
    // Return Model Textures
    return textures;
}

// Render Model
void sgl::Model::render(sgl::Shader &shader) {
    for(unsigned int i {0}; i < this->meshes.size(); i++) {
        this->meshes[i].render(shader);
    }
}

// Destroy Model - Destroys All The Meshes
void sgl::Model::destroy(void) {
    for(unsigned int i {0}; i < this->meshes.size(); i++) {
        this->meshes[i].destroy();
    }
}
