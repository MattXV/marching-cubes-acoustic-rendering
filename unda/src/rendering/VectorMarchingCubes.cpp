#include "VectorMarchingCubes.h"

namespace unda {

	std::vector<unda::Vertex> ScalarFieldVector3D::computeVertexData(double isoLevel)
	{
		std::vector<unda::Vertex> vertices;
		std::array<Triangle3D, 5> trianglesAfterPolygonisation;

		//first, polygonise the field
		//iterate over cells, not over points
		float x, y, z, u = 0.5f, v = 0.5f, nx, ny, nz;
		for (int i = 0; i < sizeX - 1; ++i)
		{
			for (int j = 0; j < sizeY - 1; ++j)
			{
				for (int k = 0; k < sizeZ - 1; ++k)
				{
					auto numTris = polygoniseCell(i, j, k, isoLevel, trianglesAfterPolygonisation);
					for (decltype(numTris) c = 0; c < numTris; ++c)
					{
						glm::vec3 normal = trianglesAfterPolygonisation[c].computeNormalVector(); 
						//dodgy version here: using the triangle normal instead of a smoothed normal at the vertices


						//this is a little inefficient, but ok enough for this
						x = trianglesAfterPolygonisation[c].a.x;
						y = trianglesAfterPolygonisation[c].a.y;
						z = trianglesAfterPolygonisation[c].a.z;
						nx = normal.x;
						ny = normal.y;
						nz = normal.z;

						vertices.emplace_back(x, y, z, u, v, nx, ny, nz);

						x = trianglesAfterPolygonisation[c].b.x;
						y = trianglesAfterPolygonisation[c].b.y;
						z = trianglesAfterPolygonisation[c].b.z;

						vertices.emplace_back(x, y, z, u, v, nx, ny, nz);

						x = trianglesAfterPolygonisation[c].c.x;
						y = trianglesAfterPolygonisation[c].c.y;
						z = trianglesAfterPolygonisation[c].c.z;

						vertices.emplace_back(x, y, z, u, v, nx, ny, nz);
					}
				}
			}
		}

		return vertices;


	}

	
	
	unsigned int ScalarFieldVector3D::polygoniseCell(size_t x, size_t y, size_t z, double isoLevel, std::array<Triangle3D, 5>& triangleResult)
	{
		int cubeindex = 0;
		std::array<Point3D, 12> vertlist{};

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


	std::array<size_t, 3> ScalarFieldVector3D::cellCornerIndexToIJKIndex(size_t vertexIndex, size_t i, size_t j, size_t k)
	{
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

		assert(vertexIndex <= 7);
		switch (vertexIndex)
		{
		case 0:
			return { i, j, k + 1 };
		case 1:
			return { i + 1, j, k + 1 };
		case 2:
			return { i + 1, j, k };
		case 3:
			return { i, j, k };
		case 4:
			return { i, j + 1, k + 1 };
		case 5:
			return { i + 1, j + 1, k + 1 };
		case 6:
			return { i + 1, j + 1, k };
		case 7:
			return { i, j + 1, k };
		default:
			return {};//shouldn't ever get here, polygonise shouldn't call for vertexIndex outside 0 to 7 - safer to put an assert here...
		}
	}



	Point3D ScalarFieldVector3D::interpolateVertex(double isoLevel, const std::array<size_t, 3>& xyzVertexA, const std::array<size_t, 3>& xyzVertexB) {
		Point3D& p1 = _cubeLattice[xyzVertexA];
		Point3D& p2 = _cubeLattice[xyzVertexB];

		double valp1 = (double)_scalarField[xyzVertexA];
		double valp2 = (double)_scalarField[xyzVertexB];

		double mu;
		Point3D p;

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



	void CubeLatticeVector::computeLatticeVertices()
	{
		//subtract default centre and shift to new centre
		float centreAdjustX = -_gridSpacing * float(sizeX - 1) / 2.0f + _centre.x;
		float centreAdjustY = -_gridSpacing * float(sizeY - 1) / 2.0f + _centre.y;
		float centreAdjustZ = -_gridSpacing * float(sizeZ - 1) / 2.0f + _centre.z;

		for (auto i = 0; i < sizeX; ++i)
		{
			for (auto j = 0; j < sizeY; ++j)
			{
				for (auto k = 0; k < sizeZ; ++k)
				{

					LatticeVector3D::getValue(i, j, k) =
					{
						i * _gridSpacing + centreAdjustX,
						j * _gridSpacing + centreAdjustY,
						k * _gridSpacing + centreAdjustZ
					};
				}
			}
		}
	}


	// -----------------------------------------------------------------------------

	void MarchingCubesWorker::calculateFieldFromMesh()
	{

	}



	// -------------------------------------------------------------------------

	MarchingCubes::MarchingCubes(int _resolution, int _nThreads, float _gridSpacing, Point3D _centre)
		: scalarField((size_t)resolution, (size_t)resolution, (size_t)resolution)
		, nThreads(_nThreads)
		, resolution(_resolution)
		, cubeLattice(_gridSpacing, _centre, (size_t)resolution, (size_t)resolution, (size_t)resolution)
	{
		if (_nThreads > resolution) _nThreads = resolution;
	}

	MarchingCubes::~MarchingCubes()
	{
		for (MarchingCubesWorker* worker : workers) {
			delete worker;
		}
	}

	/// <summary>
	/// Takes a Model with multiple meshes to create a scalar field representation based on the current resolution parameter.
	/// </summary>
	/// <param name="model">
	/// Just a weak pointer or shared pointer object.
	/// </param>
	void MarchingCubes::computeScalarField(std::weak_ptr<Model> model)
	{
		std::vector<std::thread> threads;
		for (int i = 0; i < nThreads; i++) {
			size_t stride = (size_t)floor((long double)resolution / (long double)nThreads);
			size_t startIndex = i * stride;
			size_t endIndex = (i == nThreads - 1) ? resolution : startIndex + stride;

			threads.push_back(std::thread([this, model, startIndex, endIndex]() { scalarFieldFromMeshWorker(model, startIndex, endIndex); }));
		}
		for (auto& thread : threads) {
			thread.join();
		}
	}

	void MarchingCubes::computeMarchingCubes(double isoLevel)
	{
		if (nThreads < 0 || nThreads > 32) {
			std::cerr << "[Error]: incorrect number of threads!" << std::endl;
			return;
		}

		std::vector<std::thread> threads;

		for (int i = 0; i < nThreads; i++) {
			size_t stride = (size_t)floor((long double)resolution / (long double)nThreads);
			size_t indexStart = i * stride;
			size_t indexEnd = (i == nThreads - 1) ? resolution - (size_t)1 : indexStart + stride;

			threads.push_back(std::thread([this, isoLevel, indexStart, indexEnd]() { marchingCubesWorker(isoLevel, indexStart, indexEnd); }));
		}

		for (std::thread& thread : threads) thread.join();
		std::cout << "Computed marching cubes: " << vertices.size() << std::endl;
	}

	Model* MarchingCubes::createModel()
	{
		if (vertices.empty()) {
			utils::logError("Marching Cubes: No vertices generated!", unda::errorSeverity::CRITICAL);
			return nullptr;
		}

		Model* model = new Model();
		Texture* texture = new Texture(16, 16, unda::Colour<unsigned char>(120, 120, 120, 255));
		model->addMesh(std::move(vertices), std::vector<unsigned int>(), texture, "Marching Cubes", nullptr);
		return model;
	}





	void MarchingCubes::scalarFieldFromMeshWorker(std::weak_ptr<Model> model, size_t indexStart, size_t indexEnd)
	{
		int nMeshes;
		scalarFieldMutex.lock();
		size_t resolution = scalarField.sizeX;
		scalarFieldMutex.unlock();


		std::shared_ptr<Model> model_ptr = model.lock();

		const auto& meshes = model_ptr->getMeshes();
		nMeshes = meshes.size();
		AABB* aabbs = new AABB[(size_t)nMeshes];
		for (int i = 0; i < nMeshes; i++) memcpy(&aabbs[i], &meshes[i].aabb, sizeof(AABB));

		model_ptr.reset();


		for (size_t x = 0; x < resolution; x++)
		{
			for (size_t y = indexStart; y < indexEnd; y++)
			{
				for (size_t z = 0; z < resolution; z++)
				{
					glm::vec3 samplePoint = glm::vec3(
						(float(x) / (float)resolution) * 2.0f - 1.0f,
						(float(y) / (float)resolution) * 2.0f - 1.0f,
						(float(z) / (float)resolution) * 2.0f - 1.0f);

					glm::vec3 cubeSize{ 0.0f, 0.0f, 0.0f };
					AABB sampleCube = AABB(samplePoint - 0.02f, samplePoint + .02f, samplePoint);

					float fieldValue = 0.0f;
					for (int i = 0; i < nMeshes; i++) {
						if (CheckCollision(sampleCube, aabbs[i])) {
							fieldValue = 1.0f;
							break;
						}
					}
					scalarField[std::array<size_t, 3>{x, y, z}] = fieldValue;
				}
			}
		}
		delete[] aabbs;
	}

	void MarchingCubes::marchingCubesWorker(double isoLevel, size_t indexStart, size_t indexEnd)
	{
		std::array<Triangle3D, 5> trianglesAfterPolygonisation;

		//first, polygonise the field
		//iterate over cells, not over points
		float x, y, z, u = 0.5f, v = 0.5f, nx, ny, nz;
		for (int i = 0; i < resolution - 1; ++i)
		{
			for (int j = indexStart; j < indexEnd; ++j)
			{
				for (int k = 0; k < resolution - 1; ++k)
				{
					auto numTris = polygoniseCell(i, j, k, isoLevel, trianglesAfterPolygonisation);
					for (decltype(numTris) c = 0; c < numTris; ++c)
					{
						glm::vec3 normal = trianglesAfterPolygonisation[c].computeNormalVector();
						std::array<Vertex, 3> vertexArray;
						//dodgy version here: using the triangle normal instead of a smoothed normal at the vertices

						//this is a little inefficient, but ok enough for this
						x = trianglesAfterPolygonisation[c].a.x;
						y = trianglesAfterPolygonisation[c].a.y;
						z = trianglesAfterPolygonisation[c].a.z;
						nx = normal.x;
						ny = normal.y;
						nz = normal.z;
						vertexArray[0] = Vertex(x, y, z, u, v, nx, ny, nz);

						x = trianglesAfterPolygonisation[c].b.x;
						y = trianglesAfterPolygonisation[c].b.y;
						z = trianglesAfterPolygonisation[c].b.z;
						vertexArray[1] = Vertex(x, y, z, u, v, nx, ny, nz);

						x = trianglesAfterPolygonisation[c].c.x;
						y = trianglesAfterPolygonisation[c].c.y;
						z = trianglesAfterPolygonisation[c].c.z;
						vertexArray[2] = Vertex(x, y, z, u, v, nx, ny, nz);

						verticesMutex.lock();
						for (const Vertex& vertex : vertexArray) vertices.push_back(vertex);
						verticesMutex.unlock();

					}
				}
			}
		}

	}

	unsigned int MarchingCubes::polygoniseCell(size_t x, size_t y, size_t z, double isoLevel, std::array<Triangle3D, 5>& triangleResult)
	{
		int cubeindex = 0;
		std::array<Point3D, 12> vertlist{};

		if (scalarField[cellCornerIndexToIJKIndex(0, x, y, z)] < isoLevel) cubeindex |= 1;
		if (scalarField[cellCornerIndexToIJKIndex(1, x, y, z)] < isoLevel) cubeindex |= 2;
		if (scalarField[cellCornerIndexToIJKIndex(2, x, y, z)] < isoLevel) cubeindex |= 4;
		if (scalarField[cellCornerIndexToIJKIndex(3, x, y, z)] < isoLevel) cubeindex |= 8;
		if (scalarField[cellCornerIndexToIJKIndex(4, x, y, z)] < isoLevel) cubeindex |= 16;
		if (scalarField[cellCornerIndexToIJKIndex(5, x, y, z)] < isoLevel) cubeindex |= 32;
		if (scalarField[cellCornerIndexToIJKIndex(6, x, y, z)] < isoLevel) cubeindex |= 64;
		if (scalarField[cellCornerIndexToIJKIndex(7, x, y, z)] < isoLevel) cubeindex |= 128;

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
		unsigned int ntriang = 0;
		for (auto t = 0; triTable[cubeindex][t] != -1; t += 3) {
			triangleResult[ntriang].a = vertlist[triTable[cubeindex][t]];
			triangleResult[ntriang].b = vertlist[triTable[cubeindex][t + 1]];
			triangleResult[ntriang].c = vertlist[triTable[cubeindex][t + 2]];
			ntriang++;
		}

		return(ntriang);
	}
	std::array<size_t, 3> MarchingCubes::cellCornerIndexToIJKIndex(size_t vertexIndex, size_t i, size_t j, size_t k)
	{
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

		assert(vertexIndex <= 7);
		switch (vertexIndex)
		{
		case 0:
			return { i, j, k + 1 };
		case 1:
			return { i + 1, j, k + 1 };
		case 2:
			return { i + 1, j, k };
		case 3:
			return { i, j, k };
		case 4:
			return { i, j + 1, k + 1 };
		case 5:
			return { i + 1, j + 1, k + 1 };
		case 6:
			return { i + 1, j + 1, k };
		case 7:
			return { i, j + 1, k };
		default:
			return {};//shouldn't ever get here, polygonise shouldn't call for vertexIndex outside 0 to 7 - safer to put an assert here...
		}
	}
	Point3D MarchingCubes::interpolateVertex(double isoLevel, const std::array<size_t, 3>& xyzVertexA, const std::array<size_t, 3>& xyzVertexB)
	{
		Point3D& p1 = cubeLattice[xyzVertexA];
		Point3D& p2 = cubeLattice[xyzVertexB];

		double valp1 = (double)scalarField[xyzVertexA];
		double valp2 = (double)scalarField[xyzVertexB];

		double mu;
		Point3D p;

		if (abs(isoLevel - valp1) < 0.00001)
			return(p1);
		if (abs(isoLevel - valp2) < 0.00001)
			return(p2);
		if (abs(valp1 - valp2) < 0.00001)
			return(p1);
		mu = (isoLevel - valp1) / (valp2 - valp1);
		p.x = float(p1.x + (float)mu * (p2.x - p1.x));
		p.y = float(p1.y + (float)mu * (p2.y - p1.y));
		p.z = float(p1.z + (float)mu * (p2.z - p1.z));

		return(p);
	}
}