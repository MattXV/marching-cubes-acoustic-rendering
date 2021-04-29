#pragma once

#include "../scene/Model.h"
#include "MarchingCubesTables.h"
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <assert.h>
#include <mutex>
#include <thread>


namespace unda {
	// ---------------------------------------------------------------------------

	template<typename T>
	//using T = Point3D;
	class LatticeVector3D {
	public:
		LatticeVector3D(size_t _sizeX, size_t _sizeY, size_t _sizeZ)
			: data(_sizeX * _sizeY * _sizeZ, T(), std::allocator<T>())
			, sizeX(_sizeX)
			, sizeY(_sizeY)
			, sizeZ(_sizeZ)
		{
			
		}
		//LatticeVector3D() : data{} { }
		LatticeVector3D(std::vector<T>&& latticeData) : data(latticeData) { }
		
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
		CubeLatticeVector(float gridSpacing, const Point3D& centre, size_t _sizeX, size_t _sizeY, size_t _sizeZ)
			: _gridSpacing(gridSpacing)
			, _centre(centre),
			LatticeVector3D<Point3D>(_sizeX, _sizeY, _sizeZ)
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
			, _cubeLattice(gridSpacing, centre, sizeX, sizeY, sizeZ)
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



	//template<typename T, size_t sizeX, size_t sizeY, size_t sizeZ>
	inline static void computeScalarFieldFromMeshes(LatticeVector3D<float>& data, Model* model)
	{		
		for (size_t x = 0; x < data.sizeX; ++x)
		{
			for (size_t y = 0; y < data.sizeY; ++y)
			{
				for (size_t z = 0; z < data.sizeZ; ++z)
				{
					Vertex samplePoint = Vertex((float(x) / (float)data.sizeX) * 2.0f - 1.0f,
												(float(y) / (float)data.sizeY) * 2.0f - 1.0f,
												(float(z) / (float)data.sizeZ) * 2.0f - 1.0f,
												 0.0f, 0.0f,
												0.0f, 0.0f, 0.0f);
					glm::vec3 cubeSize{ 0.00f, 0.00f, 0.00f };

					AABB sampleCube = AABB(samplePoint, samplePoint);

					float fieldValue = 0.0f;
					for (Mesh& mesh : model->getMeshes()) {
					
						if (CheckCollision(sampleCube, mesh.aabb)) {
							fieldValue = 1.0f;
							break;
						}
						else {
							fieldValue = 0.0f;
						}
					
					}
					data[std::array<size_t, 3>{x, y, z}] = fieldValue;
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	// Multithreadead Marching Cubes from a multi-mesh Model
	///////////////////////////////////////////////////////////////////////////////

	class MarchingCubesWorker {
	public:
		MarchingCubesWorker(size_t _idxStart, size_t _idxEnd,
			std::mutex& _scalarFieldMutex, std::mutex& _modelMutex,
			LatticeVector3D<float>& _scalarField, CubeLatticeVector& _cubeLattice, std::weak_ptr<Model>& _model,
			int _id = 0)
			: scalarFieldMutex(_scalarFieldMutex)
			, modelMutex(_modelMutex)
			, scalarField(_scalarField)
			, cubeLattice(_cubeLattice)
			, indexStart(_idxStart)
			, indexEnd(_idxEnd)
			, model(_model)
			, id(_id)
		{
		}
		void calculateFieldFromMesh();
	private:
		size_t indexStart, indexEnd;
		std::mutex& scalarFieldMutex;
		std::mutex& modelMutex;
		LatticeVector3D<float>& scalarField;
		CubeLatticeVector& cubeLattice;
		std::weak_ptr<Model> model;

		int id;

		// Marching Cubes Algorithm
		unsigned int polygoniseCell(size_t x, size_t y, size_t z, double isoLevel, std::array<Triangle3D, 5>& triangleResult);
		std::array<size_t, 3> cellCornerIndexToIJKIndex(size_t vertexIndex, size_t i, size_t j, size_t k);
		Point3D interpolateVertex(double isoLevel, const std::array<size_t, 3>& xyzVertexA, const std::array<size_t, 3>& xyzVertexB);
	};


	// That's the current dude
	class MarchingCubes {
	public:
		MarchingCubes(int _resolution, int _nThreads, float _gridSpacing, Point3D _centre);
		~MarchingCubes();

		void computeScalarField(std::weak_ptr<Model> model);
		void computeMarchingCubes(double isoLevel);
		LatticeVector3D<float>& getScalarField() { return scalarField; }
		Model* createModel();

	private:
		// Multithreading 
		const int nThreads, resolution;
		std::vector<MarchingCubesWorker*> workers;

		std::mutex scalarFieldMutex, modelMutex, verticesMutex;
		LatticeVector3D<float> scalarField;
		CubeLatticeVector cubeLattice;
		std::vector<Vertex> vertices;

		// Image Patch Generation
		void generateMarchedCubesPatches(const AABB& objectAABB, Texture* objectTexture, const AABB& marchedCube);

		// Workers
		void scalarFieldFromMeshWorker(std::weak_ptr<Model> model, size_t indexStart, size_t indexEnd);
		void marchingCubesWorker(double isoLevel, size_t indexStart, size_t indexEnd);

		// Marching Cubes Algorithm
		unsigned int polygoniseCell(size_t x, size_t y, size_t z, double isoLevel, std::array<Triangle3D, 5>& triangleResult);
		std::array<size_t, 3> cellCornerIndexToIJKIndex(size_t vertexIndex, size_t i, size_t j, size_t k);
		Point3D interpolateVertex(double isoLevel, const std::array<size_t, 3>& xyzVertexA, const std::array<size_t, 3>& xyzVertexB);

	};

}