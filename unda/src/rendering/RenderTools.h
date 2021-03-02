#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glad/glad.h>

#include <assert.h>
#include <iostream>
#include <vector>


namespace unda {
	class Transform {
	public:
		Transform() {
			position = glm::vec3(0, 0, 0);
			rotation = glm::vec3(0, 0, 0);
			scale = glm::vec3(1, 1, 1);
		}
		Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl) {
			position = pos;
			rotation = rot;
			scale = scl;
		}

		virtual const glm::vec3& getPosition() { return position; }
		virtual const glm::vec3& getRotation() { return rotation; }
		virtual const glm::vec3& getScale() { return scale; }

		virtual void setPosition(const glm::vec3& newPos) { position = newPos; }
		virtual void setRotation(const glm::vec3& newRot) { rotation = newRot; }
		virtual void setScale(const glm::vec3& newScale) { position = newScale; }

	private:
		glm::vec3 position, rotation, scale;
	};

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
		Vertex(float xPos, float yPos, float zPos, float uCoord, float vCoord, float xNorm, float yNorm, float zNorm) {
			x = xPos;
			y = yPos;
			z = zPos;
			u = uCoord;
			v = vCoord;
			nx = xNorm;
			ny = yNorm;
			nz = zNorm;
		}
		float x, y, z, u, v, nx, ny, nz;
	};
	static constexpr float pi = glm::pi<float>();
	unsigned int createVBO(const std::vector<Vertex>& vertices);
	unsigned int createIBO(const std::vector<unsigned int>& indices);
	glm::mat4 createModelMatrix(const glm::vec3& rotation, const glm::vec3& translation, const glm::vec3& scale);

}