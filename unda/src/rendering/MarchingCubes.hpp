#pragma once

//DKE Green | 2020 | https://www.dgr582.com
//https://www.dgr582.com/articles/2020/marching-cubes


#include <glm/glm.hpp>
#include <array>
#include "MarchingCubesTables.h"


namespace unda {
	static float computeDistanceAFromBC(glm::vec3 A, glm::vec3 B, glm::vec3 C) {
		glm::vec3 d = (C - B) / glm::distance(C, B);
		glm::vec3 v = A - B;
		float t = glm::dot(v, d);
		glm::vec3 P = B + t * d;

		return glm::distance(P, A);
	}


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
		Triangle() {
			a = Point();
			b = Point();
			c = Point();
			_vertices = { a, b, c };
		}
		Triangle(const Point& aPoint, const Point& bPoint, const Point& cPoint) 
			: _vertices { aPoint, bPoint, cPoint }
		{
			a = aPoint;
			b = bPoint;
			c = cPoint;
		}
		Point a = Point(), b = Point(), c = Point();
		std::array<Point, 3> _vertices;

		glm::vec3 computeNormalVector() {
			//sneaking glm in here, nor super clean but is easier than writing my own cross product!

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



	template<typename T, size_t sizeX, size_t sizeY, size_t sizeZ>
	class LatticeData3D {
	public:
		LatticeData3D() : data{} {}
		LatticeData3D(const std::array<T, sizeX* sizeY* sizeZ>& latticeData) : data(latticeData) {}
		~LatticeData3D() = default;

		T& operator[](size_t linearIndex) { return data[linearIndex]; }
		const T& operator[](size_t linearIndex) const { return data[linearIndex]; }
		T& operator[](std::array<size_t, 3> xyzIndex) { return data[toLinearIndex(xyzIndex[0], xyzIndex[1], xyzIndex[2])]; }

		T& getValue(size_t i, size_t j, size_t k) { return data[toLinearIndex(i, j, k)]; }
		//const T& getValue(size_t i, size_t j, size_t k) const { return data[toLinearIndex(i, j, k)]; }

	protected:
		std::array<size_t, 3> xyzSize{ sizeX, sizeY, sizeZ };
		size_t toLinearIndex(size_t xIndex, size_t yIndex, size_t zIndex) {
			return xIndex * sizeY * sizeZ + yIndex * sizeZ + zIndex;
		}
		std::array<T, sizeX* sizeY* sizeZ> data;
	};


	template<typename T, size_t sizeX, size_t sizeY, size_t sizeZ>
	void assignScalarField(LatticeData3D<T, sizeX, sizeY, sizeZ>& data, const Point centre)
	{
		for (size_t i = 0; i < sizeX; ++i)
		{
			for (size_t j = 0; j < sizeY; ++j)
			{
				for (size_t k = 0; k < sizeZ; ++k)
				{
					float dX = (i - centre.x - float(sizeX) / 2.0f);
					float dY = (j - centre.y - float(sizeY) / 2.0f);
					float dZ = (k - centre.z - float(sizeZ) / 2.0f);
					float distSq = dX * dX + dY * dY + dZ * dZ;

					auto perpDist = computeDistanceAFromBC(glm::vec3{ dX , dY , dZ }, glm::vec3{ 0,0,0 }, glm::vec3{ 1,0,0 });

					data[std::array<size_t, 3>{i, j, k}] = float(std::max(std::exp(-0.0085 * distSq) - std::exp(-0.3 * perpDist), 0.0));
				}
			}
		}
	}





	// They fill a Lattice data array. temporarily. Just testing marching cubes

 
	///////////////////////////////////////////////////////////////////////////////

	template<size_t sizeX, size_t sizeY, size_t sizeZ>
	class CubeLattice3D : public LatticeData3D<Point, sizeX, sizeY, sizeZ>
	{
	public:

		CubeLattice3D(float gridSpacing, const Point& gridCentre) 
			: _gridSpacing(gridSpacing)
			, _gridCentre(gridCentre)
			, LatticeData3D<Point, sizeX, sizeY, sizeZ>()
		{
			computeLatticeVertices();
		}
		~CubeLattice3D() = default;

	private:
		float _gridSpacing;
		const Point _gridCentre;

		void computeLatticeVertices() {
			//subtract default centre and shift to new centre
			float centreAdjustX = -_gridSpacing * float(sizeX - 1) / 2.0f + _gridCentre.x;
			float centreAdjustY = -_gridSpacing * float(sizeY - 1) / 2.0f + _gridCentre.y;
			float centreAdjustZ = -_gridSpacing * float(sizeZ - 1) / 2.0f + _gridCentre.z;

			for (auto i = 0; i < sizeX; ++i)
			{
				for (auto j = 0; j < sizeY; ++j)
				{
					for (auto k = 0; k < sizeZ; ++k)
					{

						CubeLattice3D::getValue(i, j, k) =
						{
							i * _gridSpacing + centreAdjustX,
							j * _gridSpacing + centreAdjustY,
							k * _gridSpacing + centreAdjustZ
						};
					}
				}
			}
		}
	};

	///////////////////////////////////////////////////////////////////////////////

	template<size_t sizeX, size_t sizeY, size_t sizeZ>
	class CubeLatticeScalarField3D {
	public:
		CubeLatticeScalarField3D(float gridSpacing, const Point& gridCentre) :
			_cubeLattice{ gridSpacing, gridCentre }, _scalarField{ } {}
		CubeLatticeScalarField3D(float gridSpacing, Point gridCentre, const LatticeData3D<float, sizeX, sizeY, sizeZ>& scalarField) 
			: _cubeLattice { gridSpacing, gridCentre }
			, _scalarField { scalarField }
		{

		}

	 std::vector<Vertex> computeVertexData(double isoLevel) {

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;


		std::array<Triangle, 5> trianglesAfterPolygonisation;

		//first, polygonise the field
		//iterate over cells, not over points
		unsigned int i = 0;
		float x, y, z, u, v, nx, ny, nz;
		for (auto i = 0; i < sizeX - 1; ++i)
		{
			u = (float)i / (float)(sizeX - 1);
			for (auto j = 0; j < sizeY - 1; ++j)
			{
				for (auto k = 0; k < sizeZ - 1; ++k)
				{
					v = (float)k / (float)(sizeZ - 1);

					auto numTris = polygoniseCell(i, j, k, isoLevel, trianglesAfterPolygonisation);
					for (decltype(numTris) c = 0; c < numTris; ++c)
					{
						glm::vec3 normal =  trianglesAfterPolygonisation[c].computeNormalVector(); //dodgy version here: using the triangle normal instead of a smoothed normal at the vertices


						//this is a little inefficient, but ok enough for this
						x = trianglesAfterPolygonisation[c].a.x;
						y = trianglesAfterPolygonisation[c].a.y;
						z = trianglesAfterPolygonisation[c].a.z;
						nx = normal.x;
						ny = normal.y;
						nz = normal.z;

						vertices.emplace_back(x, y, z, u, v, nx, ny, nz);
						//indices.emplace_back(c + (i * 3));
						//indices.emplace_back(c + (i * 3) + 1);
						//indices.emplace_back(c + (i  *3) + 2);
						//i++;

						x = trianglesAfterPolygonisation[c].b.x;
						y = trianglesAfterPolygonisation[c].b.y;
						z = trianglesAfterPolygonisation[c].b.z;
						vertices.emplace_back(x, y, z, u, v, nx, ny, nz);
						//indices.emplace_back(c + (i * 3));
						//indices.emplace_back(c + (i * 3) + 1);
						//indices.emplace_back(c + (i * 3) + 2);
						//i++;
						
						x = trianglesAfterPolygonisation[c].c.x;
						y = trianglesAfterPolygonisation[c].c.y;
						z = trianglesAfterPolygonisation[c].c.z;
						vertices.emplace_back(x, y, z, u, v, nx, ny, nz);
						//indices.emplace_back(c + (i * 3));
						//indices.emplace_back(c + (i * 3) + 1);
						//indices.emplace_back(c + (i * 3) + 2);
						//i++;
					}
				}
			}
		}

		return vertices;
	}

	private:
		LatticeData3D<float, sizeX, sizeY, sizeZ> _scalarField;

		CubeLattice3D<sizeX, sizeY, sizeZ> _cubeLattice;
		unsigned int polygoniseCell(size_t x, size_t y, size_t z, double isoLevel, std::array<Triangle, 5>& triangleResult) {
			int cubeindex = 0;
			std::array<Point, 12> vertlist{};

			if (_scalarField[cellCornerIndexToIJKIndex(0, x, y, z)] < isoLevel) cubeindex |= 1;
			if (_scalarField[cellCornerIndexToIJKIndex(1, x, y, z)] < isoLevel) cubeindex |= 2;
			if (_scalarField[cellCornerIndexToIJKIndex(2, x, y, z)] < isoLevel) cubeindex |= 4;
			if (_scalarField[cellCornerIndexToIJKIndex(3, x, y, z)] < isoLevel) cubeindex |= 8;
			if (_scalarField[cellCornerIndexToIJKIndex(4, x, y, z)] < isoLevel) cubeindex |= 16;
			if (_scalarField[cellCornerIndexToIJKIndex(5, x, y, z)] < isoLevel) cubeindex |= 32;
			if (_scalarField[cellCornerIndexToIJKIndex(6, x, y, z)] < isoLevel) cubeindex |= 64;
			if (_scalarField[cellCornerIndexToIJKIndex(7, x, y, z)] < isoLevel) cubeindex |= 128;

			/* Cube is entirely in/out of the surface */
			if (edgeTable[cubeindex] == 0)
				return(0);

			/* Find the vertices where the surface intersects the cube */
			if (edgeTable[cubeindex] & 1)
				vertlist[0] = interpolateVertex(isoLevel, cellCornerIndexToIJKIndex(0, x, y, z), cellCornerIndexToIJKIndex(1, x, y, z));
			if (edgeTable[cubeindex] & 2)
				vertlist[1] = interpolateVertex(isoLevel, cellCornerIndexToIJKIndex(1, x, y, z), cellCornerIndexToIJKIndex(2, x, y, z));
			if (edgeTable[cubeindex] & 4)
				vertlist[2] = interpolateVertex(isoLevel, cellCornerIndexToIJKIndex(2, x, y, z), cellCornerIndexToIJKIndex(3, x, y, z));
			if (edgeTable[cubeindex] & 8)
				vertlist[3] = interpolateVertex(isoLevel, cellCornerIndexToIJKIndex(3, x, y, z), cellCornerIndexToIJKIndex(0, x, y, z));
			if (edgeTable[cubeindex] & 16)
				vertlist[4] = interpolateVertex(isoLevel, cellCornerIndexToIJKIndex(4, x, y, z), cellCornerIndexToIJKIndex(5, x, y, z));
			if (edgeTable[cubeindex] & 32)
				vertlist[5] = interpolateVertex(isoLevel, cellCornerIndexToIJKIndex(5, x, y, z), cellCornerIndexToIJKIndex(6, x, y, z));
			if (edgeTable[cubeindex] & 64)
				vertlist[6] = interpolateVertex(isoLevel, cellCornerIndexToIJKIndex(6, x, y, z), cellCornerIndexToIJKIndex(7, x, y, z));
			if (edgeTable[cubeindex] & 128)
				vertlist[7] = interpolateVertex(isoLevel, cellCornerIndexToIJKIndex(7, x, y, z), cellCornerIndexToIJKIndex(4, x, y, z));
			if (edgeTable[cubeindex] & 256)
				vertlist[8] = interpolateVertex(isoLevel, cellCornerIndexToIJKIndex(0, x, y, z), cellCornerIndexToIJKIndex(4, x, y, z));
			if (edgeTable[cubeindex] & 512)
				vertlist[9] = interpolateVertex(isoLevel, cellCornerIndexToIJKIndex(1, x, y, z), cellCornerIndexToIJKIndex(5, x, y, z));
			if (edgeTable[cubeindex] & 1024)
				vertlist[10] = interpolateVertex(isoLevel, cellCornerIndexToIJKIndex(2, x, y, z), cellCornerIndexToIJKIndex(6, x, y, z));
			if (edgeTable[cubeindex] & 2048)
				vertlist[11] = interpolateVertex(isoLevel, cellCornerIndexToIJKIndex(3, x, y, z), cellCornerIndexToIJKIndex(7, x, y, z));

			/* Create the triangle */
			int ntriang = 0;
			for (auto t = 0; triTable[cubeindex][t] != -1; t += 3) {
				triangleResult[ntriang].a = vertlist[triTable[cubeindex][t]];
				triangleResult[ntriang].b = vertlist[triTable[cubeindex][t + 1]];
				triangleResult[ntriang].c = vertlist[triTable[cubeindex][t + 2]];
				ntriang++;
			}

			return(ntriang);
		}


		std::array<size_t, 3> cellCornerIndexToIJKIndex(size_t vertexIndex, size_t i, size_t j, size_t k) {
			//Given a grid cell IJK, work out the index of the cell corners given an index from 0 to 7
			//
			// 0 - bottom left , near face  (x            , y            , z            )
			// 1 - bottom right, near face  (x+gridSpacing, y            , z            )
			// 2 - top right   , near face  (x+gridSpacing, y            , z+gridSpacing)
			// 3 - top left    , near face  (x            , y            , z+gridSpacing)
			// 4 - bottom left , far face   (x            , y+gridSpacing, z            )
			// 5 - bottom right, far face   (x+gridSpacing, y+gridSpacing, z            )
			// 6 - top right   , far face   (x+gridSpacing, y+gridSpacing, z+gridSpacing)
			// 7 - top left    , far face   (x            , y+gridSpacing, z+gridSpacing)

			switch (vertexIndex)
			{
			case 0:
				return { i,     j,     k + 1 };
			case 1:
				return { i + 1, j,     k + 1 };
			case 2:
				return { i + 1, j,     k     };
			case 3:
				return { i,     j,     k     };
			case 4:
				return { i,     j + 1, k + 1 };
			case 5:
				return { i + 1, j + 1, k + 1 };
			case 6:
				return { i + 1, j + 1, k     };
			case 7:
				return { i,     j + 1, k     };
			default:
				assert(vertexIndex <= 7);
				return { 0, 0, 0 };
			}

		}

		Point interpolateVertex(double isoLevel, const std::array<size_t, 3>& xyzVertexA, const std::array<size_t, 3>& xyzVertexB) {
			Point& p1 = _cubeLattice[xyzVertexA];
			Point& p2 = _cubeLattice[xyzVertexB];

			auto valp1 = _scalarField[xyzVertexA];
			auto valp2 = _scalarField[xyzVertexB];

			double mu;
			Point p;

			if (abs(isoLevel - valp1) < 0.00001)
				return(p1);
			if (abs(isoLevel - valp2) < 0.00001)
				return(p2);
			if (abs(valp1 - valp2) < 0.00001)
				return(p1);
			mu = (isoLevel - valp1) / (valp2 - valp1);
			p.x = float(p1.x + mu * (p2.x - p1.x));
			p.y = float(p1.y + mu * (p2.y - p1.y));
			p.z = float(p1.z + mu * (p2.z - p1.z));

			return(p);
		}
		
	};

}
