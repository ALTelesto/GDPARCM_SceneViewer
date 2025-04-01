#pragma once

#include "Dependencies.h"

// Model3D class
class Model3D {
private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    size_t indicesSize;
    GLuint VAO, VBO, EBO;

public:
    Model3D(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices);
    ~Model3D();

    void draw(glm::vec3 pos, float x, float y, float fov, GLuint shaderProgram);

    void setPosition(glm::vec3 position);
    void setRotation(glm::vec3 rotation);
    void setScale(glm::vec3 scale);
};