#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glad/glad.h>

#include <assert.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <functional>

namespace unda {
	struct AABB {
		AABB() : min(0, 0, 0), max(0, 0, 0), size(0, 0, 0), position(0, 0, 0) {}
		AABB(const glm::vec3& _min, const glm::vec3& _max, const glm::vec3& _position)
			: min(_min)
			, max(_max)
			, size(_max - _min)
			, position(_position)
		{}
		glm::vec3 min, max, size, position;
	};

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
		virtual void setScale(const glm::vec3& newScale) { scale = newScale; }

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
		Vertex() {
			x  = 0.0f;
			y  = 0.0f;
			z  = 0.0f;
			u  = 0.0f;
			v  = 0.0f;
			nx = 0.0f;
			ny = 0.0f;
			nz = 0.0f;
		}
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


	class Listener : public Transform {
	public:
		const std::string& getMicrophoneType() const { return microphoneType; }
		void setMicrophoneType(const std::string& newType) { microphoneType = newType; }

	private:
		std::string microphoneType = "";
	};

	// ---------------------------------------------------------------------------


	static constexpr float pi = glm::pi<float>();
	unsigned int createVBO(const std::vector<Vertex>& vertices);
	unsigned int createIBO(const std::vector<unsigned int>& indices);
	glm::mat4 createModelMatrix(const glm::vec3& rotation, const glm::vec3& translation, const glm::vec3& scale);
	
	template<typename T>
	static inline void normaliseVector(std::vector<T>& _vector) {
		T max = _vector[0];
		for (const T& element : _vector) {
			if (element > max) {
				max = (T)abs(element);
			}
		}

		for (T& element : _vector) {
			element /= max;
		}
	}

	static inline void ModifyVertices(std::vector<Vertex>& vertices, std::function<void(Vertex&)>& f) {
		for (Vertex& vertex : vertices) {
			f(vertex);
		}
	}

	static inline bool pointMeshCollision(glm::vec3& point, std::pair<glm::vec3, glm::vec3>& two) // AABB - AABB collision
	{
		auto [meshMin, meshMax] = two;

		bool collisionMin = point.x >= meshMin.x && point.y >= meshMin.y && point.z >= meshMin.z;
		bool collisionMax = point.x <= meshMax.x && point.y <= meshMax.y && point.z <= meshMax.z;
		return collisionMin && collisionMax;
	}

	static inline bool CheckCollision(const AABB& one, const AABB& two) {
		bool collisionX = (one.min.x < two.max.x) && (one.max.x > two.min.x);
		bool collisionY = (one.min.y < two.max.y) && (one.max.y > two.min.y);
		bool collisionZ = (one.min.z < two.max.z) && (one.max.z > two.min.z);

		return collisionX && collisionY && collisionZ;
	}

}