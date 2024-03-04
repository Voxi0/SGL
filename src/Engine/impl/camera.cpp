#include"../Engine.hpp"

// Constructor - Initialize The Camera
sgl::Camera::Camera(const glm::vec3 &position, const glm::vec3 &front, const float &moveSpeed, const float &lookSpeed, const float &fov, const float &minFOV, const float &maxFOV) {
    this->position = position;
    this->front = front;

    this->moveSpeed = moveSpeed;
    this->lookSpeed = lookSpeed;

    this->fov = fov;
    this->minFOV = minFOV;
    this->maxFOV = maxFOV;
}

// Process Keyboard Input - Move Around
void sgl::Camera::processKeyboard(GLFWwindow* win, const float &deltaTime) {
    if(glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) {
        this->position += this->front * (this->moveSpeed * deltaTime);
    } if(glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) {
        this->position -= this->front * (this->moveSpeed * deltaTime);
    } if(glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) {
        this->position -= glm::normalize(glm::cross(this->front, this->up)) * (this->moveSpeed * deltaTime);
    } if(glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) {
        this->position += glm::normalize(glm::cross(this->front, this->up)) * (this->moveSpeed * deltaTime);
    }
}

// Process Mouse Movement (Look Around) and Scroll (Zoom In/Out)
void sgl::Camera::processMouse(const double &mousePosX, const double &mousePosY) {
    // Check if The Mouse Entered The Window For The First Time
    if(this->firstMouse) {
        this->lastX = mousePosX;
        this->lastY = mousePosY;
        this->firstMouse = false;
    }

    // Calculate Mouse Position Offset
    float xOffset {(float)mousePosX - this->lastX};
    float yOffset {this->lastY - (float)mousePosY};

    // Update Mouse Last X and Y Position
    this->lastX = (float)mousePosX;
    this->lastY = (float)mousePosY;

    // Apply Look Speed (Mouse Sensitivity) to Mouse Position Offset
    xOffset *= this->lookSpeed;
    yOffset *= this->lookSpeed;

    // Update Camera Yaw and Pitch Values
    this->yaw += xOffset;
    this->pitch += yOffset;

    // Constrain Camera Pitch So It Cannot Rotate 360 Degrees Vertically
    if(this->pitch < -89.0f) {
        this->pitch = -89.0f;
    } if(this->pitch > 89.0f) {
        this->pitch = 89.0f;
    }

    // Calculate and Set New Camera Direction
    glm::vec3 direction;
    direction.x = glm::cos(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch));
    direction.y = glm::sin(glm::radians(this->pitch));
    direction.z = glm::sin(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch));
    this->front = glm::normalize(direction);
}
void sgl::Camera::processScroll(const double &scrollPosY) {
    this->fov -= (float)scrollPosY;
    if(this->fov < this->minFOV) {
        this->fov = this->minFOV;
    } if(this->fov > this->maxFOV) {
        this->fov = this->maxFOV;
    }
}

// Getters
glm::mat4 sgl::Camera::getViewMat(void) {return glm::lookAt(this->position, this->position + this->front, this->up);}
float sgl::Camera::getFOV(void) {return this->fov;}

// Setters
void sgl::Camera::setFirstMouse(const bool &value) {this->firstMouse = value;}
