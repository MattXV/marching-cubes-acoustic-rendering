#include "RenderTools.h"

unsigned int unda::createVBO(const std::vector<Vertex>& vertices)
{
	unsigned int newBuffer;
	glGenBuffers(1, &newBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, newBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(unda::Vertex), (void*)&vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	return newBuffer;
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