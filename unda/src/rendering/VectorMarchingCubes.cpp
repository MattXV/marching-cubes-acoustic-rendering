#include "VectorMarchingCubes.h"


namespace unda {

	std::unordered_map<IBoundingBox*, std::vector<TexturePatch>> AABBPatches = std::unordered_map<IBoundingBox*, std::vector<TexturePatch>>();
	std::vector<std::pair<AABB, std::vector<TexturePatch>>> MarchingCubesPatches = std::vector<std::pair<AABB, std::vector<TexturePatch>>>();

	CubeMap::Face unda::pointIsNearestTo(glm::vec3 point)
	{
		std::map<float, CubeMap::Face> distances;
		 
		distances[glm::distance(point, glm::vec3(-1, 0, 0))] = CubeMap::Face::NEGATIVE_X;
		distances[glm::distance(point, glm::vec3(1, 0, 0))]  = CubeMap::Face::POSITIVE_X;
		distances[glm::distance(point, glm::vec3(0, 0, -1))] = CubeMap::Face::NEGATIVE_Z;
		distances[glm::distance(point, glm::vec3(0, 0, 1))]  = CubeMap::Face::POSITIVE_Z;
		distances[glm::distance(point, glm::vec3(0, -1, 0))] = CubeMap::Face::NEGATIVE_Y;
		distances[glm::distance(point, glm::vec3(0, 1, 0))]  = CubeMap::Face::POSITIVE_Y;

		return distances.begin()->second;
	}
	 

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
			//scalarFieldFromMeshWorker(model, startIndex, endIndex);
			threads.push_back(std::thread([this, model, startIndex, endIndex]() { scalarFieldFromMeshWorker(model, startIndex, endIndex); }));

		}
		for (auto& thread : threads) thread.join();
		// Resynchronised to main. Can use OpenGL now.
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


	}

	Model* MarchingCubes::createModel()
	{
		if (vertices.empty()) {
			UNDA_ERROR("Marching Cubes: No vertices generated!");
			return nullptr;
		}
		Model* model = fromVertexData(std::move(vertices), {}, "MarchingCubes");

		return model;
	}





	void MarchingCubes::generateMarchedCubesPatches(const AABB& marchedCube, Mesh& mesh)
	{
		// Here we take the current 'Marched Cube', and generate 6 square patches having their sides the size
		// of a grid cell.
		// Each of the 6 patches corresponds to a wall (or rather, a face) of the resulting volume generated from
		// the marching cube poligonisation.
		// It's an approximation, and will introduce noise in the resulting computation of the absorption volume
		// but it enables the mapping between each marching cube and acoustic materials.

		// Find the mean orientation of the surface to generate patches for the face that contributes more
		
		// 1) get distance between max and min: d1
		// 2) calculate distance between current point and min: d2
		// 3) Forget this. Well, just get the current position and add and subtract the size of a cell
		//		to get a square patch.


		glm::vec3 aabbMinPoint   = glm::vec3(mesh.aabb.min.x, mesh.aabb.min.y, mesh.aabb.min.z);
		glm::vec3 minMarchedCube = glm::vec3(marchedCube.min.x, marchedCube.min.y, marchedCube.min.z);
		glm::vec3 maxMarchedCube = glm::vec3(marchedCube.max.x, marchedCube.max.y, marchedCube.max.z);

		float minRelativeDistance = fabs(glm::distance(aabbMinPoint, minMarchedCube));
		float maxRelativeDistance = fabs(glm::distance(aabbMinPoint, maxMarchedCube));

		
		std::vector<TexturePatch> marchedCubePatches;
		CubeMap::Face patchClosestTo = pointIsNearestTo(aabbMinPoint);
		
		std::string patchName;
		switch (patchClosestTo) {
		case CubeMap::Face::NEGATIVE_X:
			patchName = "leftWall";
			break;
		case CubeMap::Face::POSITIVE_X:
			patchName = "rightWall";
			break;
		case CubeMap::Face::NEGATIVE_Z:
			patchName = "frontWall";
			break;
		case CubeMap::Face::POSITIVE_Z:
			patchName = "backWall";
			break;
		case CubeMap::Face::NEGATIVE_Y:
			patchName = "floor";
			break;
		case CubeMap::Face::POSITIVE_Y:
			patchName = "ceiling";
			break;
		}

		TexturePatch patch;
		
		if (mesh.texture->generatePatch({ mesh.aabb.UVmin.x + minRelativeDistance, mesh.aabb.UVmin.y + minRelativeDistance },
										{ mesh.aabb.UVmin.x + maxRelativeDistance, mesh.aabb.UVmin.y + maxRelativeDistance },
										mesh.name + '_' + std::to_string(uniqueId++) + '_' + patchName,
										patch, false, false)) {
			marchedCubePatches.push_back(patch);
			patch.cubeMapFace = patchClosestTo;

			MarchingCubesPatches.push_back(std::make_pair(marchedCube, marchedCubePatches));
		}
	}

	void MarchingCubes::scalarFieldFromMeshWorker(std::weak_ptr<Model> model, size_t indexStart, size_t indexEnd)
	{

		int nMeshes;
		scalarFieldMutex.lock();
		size_t resolution = scalarField.sizeX;
		scalarFieldMutex.unlock();


		std::shared_ptr<Model> model_ptr = model.lock();

		std::vector<Mesh>& meshes = model_ptr->getMeshes();
		nMeshes = meshes.size();

		std::vector<std::unique_ptr<IBoundingBox>>& boundingBoxes = IBoundingBoxRenderer::getBoundingBoxes();
		//AABBPatches.insert(std::make_pair())

		for (std::unique_ptr<IBoundingBox>& aabb : boundingBoxes) AABBPatches.insert(std::make_pair(aabb.get(), std::vector<TexturePatch>()));

		model_ptr.reset();


		for (size_t x = 0; x < scalarField.sizeX; x++)
		{
			for (size_t y = indexStart; y < indexEnd; y++)
			{
				for (size_t z = 0; z < scalarField.sizeZ; z++)
				{
					Vertex samplePoint = Vertex(
						(float(x) / (float)scalarField.sizeX) * 2.0f - 1.0f,
						(float(y) / (float)scalarField.sizeY) * 2.0f - 1.0f,
						(float(z) / (float)scalarField.sizeZ) * 2.0f - 1.0f,
						0.0f, 0.0f,
						0.0f, 0.0f, 0.0f);

					Vertex nextSamplePoint = Vertex(
						(float(x + 1) / (float)scalarField.sizeX) * 2.0f - 1.0f,
						(float(y + 1) / (float)scalarField.sizeY) * 2.0f - 1.0f,
						(float(z + 1) / (float)scalarField.sizeZ) * 2.0f - 1.0f,
						0.0f, 0.0f,
						0.0f, 0.0f, 0.0f);

					AABB sampleCube = AABB(samplePoint, nextSamplePoint);
					float fieldValue = 0.0f;
					int i = 0;
					for (Mesh& mesh : meshes) {
						if (CheckCollision(sampleCube, mesh.aabb)) {
							fieldValue = 1.0f;
							// TODO: Generate 6 patches
							// TODO: one per cube face
							if (generatePatches) {
								modelMutex.lock();
								generateMarchedCubesPatches(sampleCube, mesh);
								modelMutex.unlock();
							}
							break;
						}
						else {
							fieldValue = 0.0f;
						}
						i++;
					}
					scalarField[std::array<size_t, 3>{x, y, z}] = fieldValue;
				}
			}
		}
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