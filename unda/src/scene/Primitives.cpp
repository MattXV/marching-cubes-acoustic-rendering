#include "Primitives.h"

namespace unda {

	std::pair<std::vector<Vertex>, std::vector<unsigned int>> unda::primitives::createSphere(int resolution, float radius)
	{
		float elevationStep = pi / (float)(resolution - 1);
		float horizontalStep = (2.0f * pi) / (float)(resolution - 1);
		float inverseLength = 1.0f / radius;

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

				nx = x * inverseLength;
				ny = y * inverseLength;
				nz = z * inverseLength;

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
		return std::pair<std::vector<Vertex>, std::vector<unsigned int>>(vertices, indices);
	}


	unda::Model* primitives::cubeBoundingBox(const AABB& aabb)
	{
		std::vector<unda::Vertex> vertices;
		std::vector<unsigned int> indices;

		glm::vec3 min = glm::vec3(aabb.min.x, aabb.min.y, aabb.min.z);
		glm::vec3 max = glm::vec3(aabb.max.x, aabb.max.y, aabb.max.z);
		Point3D A_bottomleft{ min.x, min.y, min.z }, A_topleft{ min.x, max.y, min.z }, A_topright{ max.x, max.y, min.z }, A_bottomright{ max.x, min.y, min.z };
		Point3D B_bottomleft{ min.x, min.y, max.z }, B_topleft{ min.x, max.y, max.z }, B_topright{ max.x, max.y, max.z }, B_bottomright{ max.x, min.y, max.z };

		std::array<std::pair<Triangle3D, Triangle3D>, 6> faces{
			std::make_pair(Triangle3D(A_topleft, A_bottomleft, A_topright), Triangle3D(A_topright, A_bottomleft, A_bottomright)),			// Front face
			std::make_pair(Triangle3D(B_topleft, B_bottomleft, B_topright), Triangle3D(B_topright, B_bottomleft, B_bottomright)),			// Back face

			std::make_pair(Triangle3D(B_topleft, A_topleft, B_topright), Triangle3D(B_topright, A_topleft, A_topright)),					// Top face
			std::make_pair(Triangle3D(B_bottomleft, A_bottomleft, B_bottomright), Triangle3D(B_bottomright, A_bottomleft, A_bottomright)),	// Bottom face

			std::make_pair(Triangle3D(A_topleft, A_bottomleft, B_bottomleft), Triangle3D(B_topleft, A_bottomleft, B_bottomleft)),			// Left face
			std::make_pair(Triangle3D(A_topright, A_bottomright, B_topright), Triangle3D(B_topright, A_bottomright, B_bottomright))			// Right face
		};




		vertices.emplace_back(A_bottomleft.x, A_bottomleft.y, A_bottomleft.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);	// 0
		vertices.emplace_back(A_topleft.x, A_topleft.y, A_topleft.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);				// 1
		vertices.emplace_back(A_topright.x, A_topright.y, A_topright.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);			// 2 
		vertices.emplace_back(A_bottomright.x, A_bottomright.y, A_bottomright.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);	// 3
																												
		vertices.emplace_back(B_bottomleft.x, B_bottomleft.y, B_bottomleft.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);	// 4
		vertices.emplace_back(B_topleft.x, B_topleft.y, B_topleft.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);				// 5
		vertices.emplace_back(B_topright.x, B_topright.y, B_topright.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);			// 6
		vertices.emplace_back(B_bottomright.x, B_bottomright.y, B_bottomright.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);	// 7

		indices.push_back(1); // Front face
		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(2);
		indices.push_back(0);
		indices.push_back(3);

		indices.push_back(5); // Back face
		indices.push_back(4);
		indices.push_back(6);
		indices.push_back(6);
		indices.push_back(4);
		indices.push_back(7);

		indices.push_back(5); // Top face
		indices.push_back(1);
		indices.push_back(6);
		indices.push_back(6);
		indices.push_back(1);
		indices.push_back(2);

		indices.push_back(4); // Bottom face
		indices.push_back(0);
		indices.push_back(7);
		indices.push_back(7);
		indices.push_back(0);
		indices.push_back(3);

		indices.push_back(1); // Left face
		indices.push_back(0);
		indices.push_back(4);
		indices.push_back(0);
		indices.push_back(4);
		indices.push_back(3);

		indices.push_back(2); // Right face
		indices.push_back(0);
		indices.push_back(3);
		indices.push_back(6);
		indices.push_back(3);
		indices.push_back(7);

		Model* model = new Model(std::move(vertices), std::move(indices));
		return model;
	}


	Model* unda::primitives::createSphereModel(int resolution, float radius)
	{
		auto [vertices, indices] = unda::primitives::createSphere(resolution, radius);
		Texture* texture = new Texture(1024, 1024, unda::Colour<unsigned char>(70, 70, 70, 255));
		return new unda::Model(std::move(vertices), std::move(indices), texture);
	}

}