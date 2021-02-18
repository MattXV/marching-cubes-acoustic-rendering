#pragma once

#include <glad/glad.h>

#include <assert.h>
#include <iostream>
#include <vector>


namespace unda {
	template<typename T>
	struct Colour {
		Colour(T red, T green, T blue, T alpha) {
			r = red;
			g = green;
			b = blue;
			a = alpha;
		}
		T r, g, b, a;
	};

	struct Vertex {
		Vertex(float xPos, float yPos, float zPos, float uCoord, float vCoord) {
			x = xPos;
			y = yPos;
			z = zPos;
			u = uCoord;
			v = vCoord;
		}
		float x, y, z, u, v;
	};

	unsigned int createVBO(const std::vector<Vertex>& vertices);
	unsigned int createIBO(const std::vector<unsigned int>& indices);
}