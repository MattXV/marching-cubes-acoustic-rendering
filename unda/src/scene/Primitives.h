#pragma once

#include "../scene/Model.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>


namespace unda {
	struct Point {
		Point() {}
		Point(float xPos, float yPos, float zPos) {
			x = xPos;
			y = yPos;
			z = zPos;
		}
		float x = 0.0f, y = 0.0f, z = 0.0f;
	};
	struct Triangle {
		Triangle() {}
		Triangle(const Point& aPoint, const Point& bPoint, const Point& cPoint) {
			a = aPoint;
			b = bPoint;
			c = cPoint;
		}
		Point a = Point(), b = Point(), c = Point();
	};

	template<typename T, size_t sizeX, size_t sizeY, size_t sizeZ>
	class LatticeData3 {
	public:
		LatticeData3() {}
		LatticeData3(const std::array<T, sizeX* sizeY* sizeZ>& latticeData) : data(latticeData) {}
		~LatticeData3() = default;
	private:
		std::array<T, sizeX * sizeY * sizeZ> data;
	};



	namespace primitives {
		unda::Model* createSphereModel(int resolution, float radius);
		std::pair<std::vector<Vertex>, std::vector<unsigned int>> createSphere(int resolution, float radius);
	}
}