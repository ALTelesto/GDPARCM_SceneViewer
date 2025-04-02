#pragma once

#include "Dependencies.h"

// Model3D class
class Model3D {
private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    size_t indicesSize;
    GLuint VAO, VBO, EBO;

    bool glInitialized = false;

public:
    Model3D(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices);
    ~Model3D();

    void draw(glm::vec3 pos, float x, float y, float fov, GLuint shaderProgram);

    void setPosition(glm::vec3 position);
    void setRotation(glm::vec3 rotation);
    void setScale(glm::vec3 scale);

    glm::vec3 getPosition();
    glm::vec3 getRotation();
    glm::vec3 getScale();

    std::vector<GLfloat> getVertices();
    std::vector<GLuint> getIndices();
    size_t getIndicesSize();

    void initializeGL();
};