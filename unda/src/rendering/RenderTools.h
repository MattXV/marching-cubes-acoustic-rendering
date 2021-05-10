#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glad/glad.h>

#include <array>
#include <assert.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <functional>


namespace unda {
	struct Vertex {
		Vertex() = default;
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
		float x = 0.0f, y = 0.0f, z = 0.0f;
		float u = 0.0f, v = 0.0f;
		float nx = 0.0f, ny = 0.0f, nz = 0.0f;
		Vertex& operator=(const Vertex& other) {
			if (this == &other) return *this;
			x = other.x;
			y = other.y;
			z = other.z;
			u = other.u;
			v = other.v;
			nx = other.nx;
			ny = other.ny;
			nz = other.nz;
			return *this;
		}
	};

	struct Point3D {
		Point3D() {}
		Point3D(float xPos, float yPos, float zPos) {
			x = xPos;
			y = yPos;
			z = zPos;
		}
		float x = 0.0f, y = 0.0f, z = 0.0f;
	};
	struct Triangle3D {
		Triangle3D() {
			a = Point3D();
			b = Point3D();
			c = Point3D();
		}
		Triangle3D(const Point3D& aPoint, const Point3D& bPoint, const Point3D& cPoint)
		{
			a = aPoint;
			b = bPoint;
			c = cPoint;
		}
		Point3D a = Point3D(), b = Point3D(), c = Point3D();

		inline glm::vec3 computeNormalVector() {
			//Dir = (B - A) x(C - A)
			//Norm = Dir / len(Dir)
			glm::vec3 dir = glm::cross(
				(glm::vec3{ b.x, b.y, b.z } - glm::vec3{ a.x, a.y, a.z }),
				(glm::vec3{ c.x, c.y, c.z } - glm::vec3{ a.x, a.y, a.z })
			);
			glm::vec3 retNormal = glm::normalize(dir);
			return retNormal;
		}

		inline std::array<Vertex, 3> getVertices() {
			glm::vec3 normal = computeNormalVector();
			std::array<Vertex, 3> vertices;
			vertices[0] = Vertex(a.x, a.y, a.z, 0.0f, 0.0f, normal.x, normal.y, normal.z);
			vertices[1] = Vertex(b.x, b.y, b.z, 0.0f, 0.0f, normal.x, normal.y, normal.z);
			vertices[2] = Vertex(c.x, c.y, c.z, 0.0f, 0.0f, normal.x, normal.y, normal.z);
			return vertices;
		}
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



	struct AABB {
		AABB() = default;
		AABB(const Vertex& _min, const Vertex& _max)
			: min(_min)
			, max(_max) {};
		~AABB() = default;
		Vertex min = Vertex(), max = Vertex();
		Vertex nearBottomLeft, nearBottomRight, nearTopRight, nearTopLeft;
		Vertex farBottomLeft, farBottomRight, farTopRight, farTopLeft;


		glm::vec3 surfaceNormal = glm::vec3();
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