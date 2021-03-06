#pragma once

#include "RenderTools.h"
#include "MarchingCubesTables.h"
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <assert.h>


namespace undaTesting {
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

		glm::vec3 computeNormalVector() {
			//Dir = (B - A) x(C - A)
			//Norm = Dir / len(Dir)
			glm::vec3 dir = glm::cross(
				(glm::vec3{ b.x, b.y, b.z } - glm::vec3{ a.x, a.y, a.z }),
				(glm::vec3{ c.x, c.y, c.z } - glm::vec3{ a.x, a.y, a.z })
			);
			glm::vec3 retNormal = glm::normalize(dir);
			return retNormal;
		}
	};

	// ---------------------------------------------------------------------------

	template<typename T>
	//using T = Point3D;
	class LatticeVector3D {
	public:
		LatticeVector3D() : data{} { }
		LatticeVector3D(const std::vector<T>& latticeData) { data.swap(latticeData); }
		
		T& operator[](size_t linearIndex) { return data[linearIndex]; }
		const T& operator[](size_t linearIndex) const { return data[linearIndex]; }
		T& operator[](std::array<size_t, 3> ijkIndex) { size_t idx = toLinearIndex(ijkIndex); return data[idx]; }
		
		T& getValue(size_t i, size_t j, size_t k) { return data[toLinearIndex({ i, j, k })]; }
		const T& getValue(size_t i, size_t j, size_t k) const { return data[toLinearIndex(std::array<size_t, 3>{i, j, k})]; }

		size_t sizeX, sizeY, sizeZ;

		void assignFloatVectors(const std::vector<float>& verticesX, const std::vector<float>& verticesY, const std::vector<float>& verticesZ)
		{
			sizeX = verticesX.size();
			sizeY = verticesY.size();
			sizeZ = verticesZ.size();
			assert((sizeX == sizeY) && (sizeY == sizeZ));
			for (int i = 0; i < verticesX.size(); i++) {
				data.emplace_back(verticesX[i]);
				data.emplace_back(verticesY[i]);
				data.emplace_back(verticesZ[i]);
			}
		}
	private:
		std::vector<T> data;

		size_t toLinearIndex(const std::array<size_t, 3>& ijkIndex) { 
			return ijkIndex[0] * sizeY * sizeZ + ijkIndex[1] * sizeZ + ijkIndex[2];
		}
	};


	// ---------------------------------------------------------------------------

	class CubeLatticeVector : public LatticeVector3D<Point3D> {
	public:
		CubeLatticeVector(float gridSpacing, const Point3D& centre) 
			: _gridSpacing(gridSpacing)
			, _centre(centre),
			LatticeVector3D<Point3D>()
		{
			computeLatticeVertices();
		}
	private:
		float _gridSpacing;
		Point3D _centre;
		void computeLatticeVertices();
	};


	// ---------------------------------------------------------------------------



	class ScalarFieldVector3D {
	public:
		ScalarFieldVector3D(float gridSpacing, const Point3D& centre, LatticeVector3D<float> latticeData)
			: _gridSpacing(gridSpacing)
			, _centre(centre)
			, _scalarField {latticeData }
			, sizeX(latticeData.sizeX)
			, sizeY(latticeData.sizeY)
			, sizeZ(latticeData.sizeZ)
			, _cubeLattice(gridSpacing, centre)
		{
			std::cout << _scalarField.sizeX << std::endl;
		}
	
		std::vector<unda::Vertex> computeVertexData(double isoLevel);

	private:
		size_t sizeX, sizeY, sizeZ;
		float _gridSpacing;
		Point3D _centre;
		LatticeVector3D<float> _scalarField;
		CubeLatticeVector _cubeLattice;

		std::array<size_t, 3> cellCornerIndexToIJKIndex(size_t vertexIndex, size_t i, size_t j, size_t k);
		unsigned int polygoniseCell(size_t x, size_t y, size_t z, double isoLevel, std::array<Triangle3D, 5>& triangleResult);
		Point3D interpolateVertex(double isoLevel, const std::array<size_t, 3>& xyzVertexA, const std::array<size_t, 3>& xyzVertexB);
	};
}