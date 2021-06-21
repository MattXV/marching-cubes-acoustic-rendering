#include "RenderTools.h"

namespace unda {
	unsigned int unda::createVBO(const std::vector<Vertex>& vertices)
	{
		unsigned int newBuffer;
		glGenBuffers(1, &newBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, newBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(unda::Vertex), (void*)&vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		return newBuffer;
	}

	glm::mat4 unda::createModelMatrix(const glm::vec3& rotation, const glm::vec3& translation, const glm::vec3& scale)
	{
		glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity matrix
		modelMatrix = glm::translate(modelMatrix, translation);
		modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::scale(modelMatrix, scale);
		return modelMatrix;
	}

	unsigned int unda::createIBO(const std::vector<unsigned int>& indices)
	{
		unsigned int newBuffer;
		glGenBuffers(1, &newBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), (void*)&indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
		return newBuffer;
	}


	/*std::array<unda::Vertex, 6> Triangle3D::getVertices()
	{
		return std::array<unda::Vertex, 6>();
	}*/

}