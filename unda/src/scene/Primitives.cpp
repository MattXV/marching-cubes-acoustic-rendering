#include "Primitives.h"

unda::Model* unda::primitives::createSphere(int resolution, float radius)
{
	float elevationStep = pi / (float)(resolution - 1);
	float horizontalStep = (2.0f * pi) / (float)(resolution - 1);
	float inverseLenght = 1.0f / radius;

	std::vector<unda::Vertex> vertices;
	float x, y, z, u, v, nx, ny, nz;
	for (int longitude = 0; longitude < resolution + 1; longitude++) {
		for (int latitude = 0; latitude < resolution + 1; latitude++) {

			float theta = (pi / 2) - (longitude * elevationStep);
			float phi = latitude * horizontalStep;

			y = radius * sinf(theta);
			x = (radius * cosf(theta)) * cosf(phi);
			z = (radius * cosf(theta)) * sinf(phi);

			u = (float)latitude / (float)resolution;
			v = (float)(longitude) / (float)resolution;

			nx = x * inverseLenght;
			ny = y * inverseLenght;
			nz = z * inverseLenght;

			vertices.emplace_back(x, y, z, u, v, nx, ny, nz);
		}
	}

	std::vector<unsigned int> indices;
	unsigned int c, n;
	for (int latitude = 0; latitude < resolution; latitude++) {

		c = latitude * (resolution + 1);
		n = c + resolution + 1;

		for (int longitude = 0; longitude < resolution; longitude++) {
			if (longitude != 0) {
				indices.push_back(c);
				indices.push_back(n);
				indices.push_back(c + 1);
			}
			if (longitude != resolution - 1) {
				indices.push_back(c + 1);
				indices.push_back(n);
				indices.push_back(n + 1);
			}
			n++;
			c++;
		}
	}

	unsigned int vbo = unda::createVBO(vertices);
	unsigned int ibo = unda::createIBO(indices);
	unsigned int indexCount = indices.size();
	Texture* texture = new Texture(1024, 1024, unda::Colour<unsigned char>(70, 70, 70, 255));

	return new unda::Model(vbo, ibo, indexCount, texture);
}
