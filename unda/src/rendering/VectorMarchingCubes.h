#pragma once

#include "../scene/Model.h"
#include "../rendering/Renderer.h"
#include "MarchingCubesTables.h"
#include "../utils/Utils.h"
#include <glm/glm.hpp>
#include <cmath>
#include <vector>
#include <array>
#include <assert.h>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <map>



namespace unda {
	//// --------------------------------Global Texture Patches---------------------
	//extern std::unordered_map<IBoundingBox*, std::vector<TexturePatch>> AABBPatches;
	//extern std::vector<std::pair<AABB, std::vector<TexturePatch>>> MarchingCubesPatches;
	//// ---------------------------------------------------------------------------

	struct Cell : public Point3D {
		Cell() : Point3D() {}
		Cell(float x, float y, float z) : Point3D(x, y, z) {}
		float value = 0.0f;
		Mesh* mesh;
	};

	class CellRenderer {
	public:
		CellRenderer(Model* _model);
		~CellRenderer() = default;
		
		inline void setOrthoVolume(float _left, float _right, float _bottom, float _top, float _zNear, float _zFar) {
			left = _left; right = _right; bottom = _bottom; top = _top; zNear = _zNear; _zFar;
		};
		inline void setCameraTarget(const glm::vec3 newTarget) { target = newTarget; }
		inline void setCameraPosition(const glm::vec3& newPosition) { position = newPosition; }
		inline void update() {
			view = glm::lookAt<float>(position, position + target, up);
			//projection = glm::ortho<float>(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
			projection = glm::ortho<float>(left, right, bottom, top, zNear, zFar);
			//projection = glm::perspective<float>(glm::radians(40.0f), 1.0f, 0.0000001f, 100.2f);
		}
		void setModel(Model* newModel) { model = newModel; }
		void render();
		void writeImage(const std::string& fileName);
	private:
		float left = 0.0f, right = 0.0f, bottom = 0.0f, top = 0.0f, zNear = 0.0f, zFar = 0.0f;
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		glm::vec3 position = glm::vec3(0.0f), target = glm::vec3(1.0f), up = glm::vec3(0.0f, 1.0f, 0.0f);

		Model* model;
		//Camera orthoCamera;
		Shader shaderProgram;
		FrameBuffer frameBuffer;
		DISABLE_COPY_ASSIGN(CellRenderer)
	};

	class LatticeVector3D {
	public:
		LatticeVector3D(size_t _sizeX, size_t _sizeY, size_t _sizeZ)
			: data(_sizeX * _sizeY * _sizeZ, Cell(), std::allocator<Cell>())
			, sizeX(_sizeX)
			, sizeY(_sizeY)
			, sizeZ(_sizeZ)
		{
			
		}

		//LatticeVector3D() : data{} { }
		LatticeVector3D(std::vector<Cell>&& latticeData) : data(latticeData) { }
		
		Cell& operator[](size_t linearIndex) { return data[linearIndex]; }
		const Cell& operator[](size_t linearIndex) const { return data[linearIndex]; }
		Cell& operator[](std::array<size_t, 3> ijkIndex) { size_t idx = toLinearIndex(ijkIndex); return data[idx]; }
		
		Cell& getValue(size_t i, size_t j, size_t k) { return data[toLinearIndex({ i, j, k })]; }
		const Cell& getValue(size_t i, size_t j, size_t k) const { return data[toLinearIndex({ i, j, k })]; }

		size_t sizeX, sizeY, sizeZ;

	private:
		std::vector<Cell> data;
		size_t toLinearIndex(std::array<size_t, 3> ijkIndex) const { 
			return ijkIndex[0] * sizeY * sizeZ + ijkIndex[1] * sizeZ + ijkIndex[2];
		}
	};


	class CubeLatticeVector : public LatticeVector3D {
	public:
		CubeLatticeVector(float gridSpacing, const Point3D& centre, size_t _sizeX, size_t _sizeY, size_t _sizeZ)
			: _gridSpacing(gridSpacing)
			, _centre(centre)
			, LatticeVector3D(_sizeX, _sizeY, _sizeZ)
		{
			computeLatticeVertices();
		}
	private:
		float _gridSpacing;
		Point3D _centre;
		void computeLatticeVertices();
	};


	class ScalarFieldVector3D {
	public:
		ScalarFieldVector3D(float gridSpacing, const Point3D& centre, LatticeVector3D latticeData)
			: _gridSpacing(gridSpacing)
			, _centre(centre)
			, _scalarField { latticeData }
			, sizeX(latticeData.sizeX)
			, sizeY(latticeData.sizeY)
			, sizeZ(latticeData.sizeZ)
			, _cubeLattice(gridSpacing, centre, sizeX, sizeY, sizeZ)
		{	
		}
	
	private:
		size_t sizeX, sizeY, sizeZ;
		float _gridSpacing;
		Point3D _centre;
		LatticeVector3D _scalarField;
		CubeLatticeVector _cubeLattice;
	};

	class MarchingCubes {
	public:
		MarchingCubes(int _resolution, int _nThreads, float _gridSpacing, Point3D _centre);
		~MarchingCubes();

		void computeScalarField(std::weak_ptr<Model> model);
		void setGeneratePatches(bool doIGeneratePatches) { generatePatches = doIGeneratePatches; }
		void computeMarchingCubes(double isoLevel);
		LatticeVector3D& getScalarField() { return scalarField; }
		Model* createModel();

	private:
		// Multithreading 
		const int nThreads, resolution;
		unsigned int uniqueId;

		std::mutex scalarFieldMutex, modelMutex, verticesMutex;
		LatticeVector3D scalarField;
		CubeLatticeVector cubeLattice;
		std::vector<Vertex> vertices;

		// Image Patch Generation
		bool generatePatches = true;
		double scale = 0.0;
		size_t nPatches = 0;
		CellRenderer cellRenderer;

		// Workers
		void cellImagePatch(size_t x, size_t y, size_t z, CubeMap::Face face);
		void scalarFieldFromMeshWorker(std::weak_ptr<Model> model, size_t indexStart, size_t indexEnd);
		void marchingCubesWorker(double isoLevel, size_t indexStart, size_t indexEnd);

		// Marching Cubes Algorithm
		unsigned int polygoniseCell(size_t x, size_t y, size_t z, double isoLevel, std::array<Triangle3D, 5>& triangleResult);
		std::array<size_t, 3> cellCornerIndexToIJKIndex(size_t vertexIndex, size_t i, size_t j, size_t k);
		Point3D interpolateVertex(double isoLevel, const std::array<size_t, 3>& xyzVertexA, const std::array<size_t, 3>& xyzVertexB);
	
		enum Corner : int {
			nearBottomLeft  = 1,
			nearBottomRight = 2,
			nearTopRight    = 4,
			nearTopLeft     = 8,
			farBottomLeft   = 16,
			farBottomRight  = 32,
			farTopRight     = 64,
			farTopLeft      = 128
		};
	
	};


	CubeMap::Face pointIsNearestTo(glm::vec3 point);
}