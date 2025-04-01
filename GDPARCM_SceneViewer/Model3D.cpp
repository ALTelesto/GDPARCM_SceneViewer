#include "Model3D.h"

Model3D::Model3D(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, const std::vector<GLfloat>& vertices,
	const std::vector<GLuint>& indices)
{
	this->vertices = vertices;
	this->indices = indices;
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;

    indicesSize = indices.size();

    // Initialize VAO, VBO, and EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Model3D::~Model3D()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Model3D::draw(glm::vec3 pos, float x, float y, float fov, GLuint shaderProgram)
{
	// axis
	float axis_x = 0.0;
	float axis_y = 1.0;
	float axis_z = 0.0;

	// camera stuff
	glm::mat4 cameraPosMatrix = glm::translate(glm::mat4(1.0f), pos * -1.0f);
	cameraPosMatrix = glm::rotate(cameraPosMatrix, glm::radians(x), glm::normalize(glm::vec3(axis_x, axis_y, axis_z)));
	cameraPosMatrix = glm::rotate(cameraPosMatrix, glm::radians(y), glm::normalize(glm::vec3(axis_y, axis_x, axis_z)));

	glm::vec3 worldUp = glm::normalize(glm::vec3(0, 1.0f, 0));

	glm::vec3 cameraCenter;
	cameraCenter.x = cos(glm::radians(x)) * cos(glm::radians(y));
	cameraCenter.y = sin(glm::radians(y));
	cameraCenter.z = sin(glm::radians(x)) * cos(glm::radians(y));
	cameraCenter = pos + cameraCenter;

	glm::mat4 viewMatrix = glm::lookAt(pos, cameraCenter, worldUp);
	glm::mat4 projection = glm::perspective(glm::radians(fov), 600.0f / 600.0f, 0.1f, 100.0f);

	// applying of model transformation
	glm::mat4 transformation_matrix = glm::translate(glm::mat4(1.0f), this->position);
	transformation_matrix = glm::rotate(transformation_matrix, glm::radians(this->rotation.x), glm::normalize(glm::vec3(axis_x, axis_y, axis_z)));
	transformation_matrix = glm::rotate(transformation_matrix, glm::radians(this->rotation.y), glm::normalize(glm::vec3(axis_y, axis_x, axis_z)));
	transformation_matrix = glm::scale(transformation_matrix, this->scale);

	// sending transformation matrix to shader program
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

	unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// set color
	glm::vec4 FragColorIn = glm::vec4(0.0, 1.0, 0.0, 1.0);
	unsigned int colorLoc = glGetUniformLocation(shaderProgram, "FragColorIn");
	glUniform4fv(colorLoc, 1, glm::value_ptr(FragColorIn));

	// rendering
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
}

void Model3D::setPosition(glm::vec3 position)
{
	this->position = position;
}

void Model3D::setRotation(glm::vec3 rotation)
{
	this->rotation = rotation;
}

void Model3D::setScale(glm::vec3 scale)
{
	this->scale = scale;
}

glm::vec3 Model3D::getPosition()
{
	return this->position;
}

glm::vec3 Model3D::getRotation()
{
	return this->rotation;
}

glm::vec3 Model3D::getScale()
{
	return this->scale;
}

std::vector<GLfloat> Model3D::getVertices()
{
	return this->vertices;
}

std::vector<GLuint> Model3D::getIndices()
{
	return this->indices;
}

size_t Model3D::getIndicesSize()
{
	return this->indicesSize;
}
