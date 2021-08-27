#include "VectorMarchingCubes.h"


namespace unda {

	//std::unordered_map<IBoundingBox*, std::vector<TexturePatch>> AABBPatches = std::unordered_map<IBoundingBox*, std::vector<TexturePatch>>();
	//std::vector<std::pair<AABB, std::vector<TexturePatch>>> MarchingCubesPatches = std::vector<std::pair<AABB, std::vector<TexturePatch>>>();

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



	// -------------------------------------------------------------------------


	MarchingCubes::MarchingCubes(int _resolution, int _nThreads, float _gridSpacing, Point3D _centre)
		: scalarField((size_t)resolution, (size_t)resolution, (size_t)resolution)
		, nThreads(_nThreads)
		, resolution(_resolution)
		, cubeLattice(_gridSpacing, _centre, (size_t)resolution, (size_t)resolution, (size_t)resolution)
		, cellRenderer(nullptr)
		, scale(0.0)
		, gridSpacing(_gridSpacing)
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
		std::shared_ptr<Model> lockedModel = model.lock();
		scale = lockedModel->getModelScale();
		cellRenderer.setModel((Model*)lockedModel.get());
		scalarFieldFromMeshWorker(model, 0, resolution);
		//std::vector<std::thread> threads;
		//for (int i = 0; i < nThreads; i++) {
		//	size_t stride = (size_t)floor((long double)resolution / (long double)nThreads);
		//	size_t startIndex = i * stride;
		//	size_t endIndex = (i == nThreads - 1) ? resolution : startIndex + stride;
		//	//scalarFieldFromMeshWorker(model, startIndex, endIndex);
		//	threads.push_back(std::thread([this, model, startIndex, endIndex]() { scalarFieldFromMeshWorker(model, startIndex, endIndex); }));

		//}
		//for (auto& thread : threads) thread.join();
		// Resynchronised to main. Can use OpenGL now.
	}

	void MarchingCubes::computeMarchingCubes(double isoLevel)
	{
		//std::vector<std::thread> threads;

		//for (int i = 0; i < nThreads; i++) {
		//	size_t stride = (size_t)floor((long double)resolution / (long double)nThreads);
		//	size_t indexStart = i * stride;
		//	size_t indexEnd = (i == nThreads - 1) ? resolution - (size_t)1 : indexStart + stride;

		//	threads.push_back(std::thread([this, isoLevel, indexStart, indexEnd]() { marchingCubesWorker(isoLevel, indexStart, indexEnd); }));
		//}
		//for (std::thread& thread : threads) thread.join();
		marchingCubesWorker(isoLevel, 0, resolution - 1);
	}

	Model* MarchingCubes::createModel()
	{
		if (vertices.empty()) {
			UNDA_ERROR("Marching Cubes: No vertices generated!");
			return nullptr;
		}
		if (loadedTextures.find("Black") == loadedTextures.end())
			loadedTextures["Black"] = std::make_unique<Texture>(1, 1, Colour<unsigned char>(0, 0, 0, 255));
		Texture* texture = loadedTextures["Black"].get();
		Model* model = fromVertexData(std::move(vertices), {}, "MarchingCubes", texture);
		return model;
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

		//for (std::unique_ptr<IBoundingBox>& aabb : boundingBoxes) AABBPatches.insert(std::make_pair(aabb.get(), std::vector<TexturePatch>()));

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
					
					Cell& fieldCell = scalarField[std::array<size_t, 3>{x, y, z}];

					AABB sampleCube = AABB(samplePoint, nextSamplePoint);
					float fieldValue = 0.0f;
					int i = 0;
					for (Mesh& mesh : meshes) {
						if (CheckCollision(sampleCube, mesh.aabb)) {
							fieldValue = 1.0f;
							// TODO: Generate 6 patches
							// TODO: one per cube face
							// 

							//if (generatePatches) {
							//	modelMutex.lock();
							//	generateMarchedCubesPatches(sampleCube, mesh);
							//	modelMutex.unlock();
							//}
							fieldCell.mesh = &mesh;
							break;
						}
						else {
							fieldCell.mesh = nullptr;
							fieldValue = 0.0f;
						}
						i++;
					}
					fieldCell.value = fieldValue;
				}
			}
		}
	}


	void MarchingCubes::marchingCubesWorker(double isoLevel, size_t indexStart, size_t indexEnd)
	{
		std::array<Triangle3D, 5> trianglesAfterPolygonisation;
		glm::vec3 normal;

		float x, y, z, u = 0.5f, v = 0.5f, nx, ny, nz;
		for (int i = 0; i < resolution - 1; ++i)
		{
			for (int j = indexStart; j < indexEnd; ++j)
			{
				for (int k = 0; k < resolution - 1; ++k)
				{
					unsigned int numTris = polygoniseCell(i, j, k, isoLevel, trianglesAfterPolygonisation);
					for (unsigned int c = 0; c < numTris; ++c)
					{
						normal = trianglesAfterPolygonisation[c].computeNormalVector();
						std::array<Vertex, 3> vertexArray;

						x = trianglesAfterPolygonisation[c].a.x;
						y = trianglesAfterPolygonisation[c].a.y;
						z = trianglesAfterPolygonisation[c].a.z;

						vertexArray[0] = Vertex(x, y, z, u, v, normal.x, normal.y, normal.z);

						x = trianglesAfterPolygonisation[c].b.x;
						y = trianglesAfterPolygonisation[c].b.y;
						z = trianglesAfterPolygonisation[c].b.z;
						vertexArray[1] = Vertex(x, y, z, u, v, normal.x, normal.y, normal.z);

						x = trianglesAfterPolygonisation[c].c.x;
						y = trianglesAfterPolygonisation[c].c.y;
						z = trianglesAfterPolygonisation[c].c.z;
						vertexArray[2] = Vertex(x, y, z, u, v, normal.x, normal.y, normal.z);

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
		int cubeindex = 0, floor = 0;
		std::array<Point3D, 12> vertlist{};
																				         // This could be a 3 bit int
		if (scalarField[cellCornerIndexToIJKIndex(0, x, y, z)].value > isoLevel) { cubeindex |= nearBottomLeft;  }   // 0 - bottom left   , near face
		if (scalarField[cellCornerIndexToIJKIndex(1, x, y, z)].value > isoLevel) { cubeindex |= nearBottomRight; }   // 1 - bottom right	, near face
		if (scalarField[cellCornerIndexToIJKIndex(2, x, y, z)].value > isoLevel) { cubeindex |= nearTopRight;    }   // 2 - top right   	, near face
		if (scalarField[cellCornerIndexToIJKIndex(3, x, y, z)].value > isoLevel) { cubeindex |= nearTopLeft;     }   // 3 - top left    	, near face
		if (scalarField[cellCornerIndexToIJKIndex(4, x, y, z)].value > isoLevel) { cubeindex |= farBottomLeft;   }  // 4 - bottom left 	, far face 
		if (scalarField[cellCornerIndexToIJKIndex(5, x, y, z)].value > isoLevel) { cubeindex |= farBottomRight;  }  // 5 - bottom right	, far face 
		if (scalarField[cellCornerIndexToIJKIndex(6, x, y, z)].value > isoLevel) { cubeindex |= farTopRight;     }  // 6 - top right   	, far face 
		if (scalarField[cellCornerIndexToIJKIndex(7, x, y, z)].value > isoLevel) { cubeindex |= farTopLeft;      } // 7 - top left    	, far face 
		// Cube is entirely in/out of the surface 
		if (edgeTable[cubeindex] == 0)
			return(0);

		if (generatePatches) {
			if (cubeindex == (nearBottomLeft + nearBottomRight + farBottomLeft + farBottomRight)) cellImagePatch(x, y, z, CubeMap::Face::POSITIVE_Y); // Floor
			if (cubeindex == (nearTopRight + nearTopLeft + farTopRight + farTopLeft))			  cellImagePatch(x, y, z, CubeMap::Face::NEGATIVE_Y); // Ceiling
			if (cubeindex == (nearBottomLeft + nearTopLeft + farBottomLeft + farTopLeft))		  cellImagePatch(x, y, z, CubeMap::Face::NEGATIVE_X); // Left
			if (cubeindex == (nearBottomRight + nearTopRight + farBottomRight + farTopRight))     cellImagePatch(x, y, z, CubeMap::Face::POSITIVE_X); // Right
			if (cubeindex == (nearBottomLeft + nearBottomRight + nearTopRight + nearTopLeft))     cellImagePatch(x, y, z, CubeMap::Face::POSITIVE_Z); // Front
			if (cubeindex == (farBottomLeft + farBottomRight + farTopRight + farTopLeft))         cellImagePatch(x, y, z, CubeMap::Face::NEGATIVE_Z); // Back
				
		}


		// Work out the vertex values interpolating across edges of the cube. 
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

		// Create the triangle 
		unsigned int ntriang = 0;
		for (auto t = 0; triTable[cubeindex][t] != -1; t += 3) {
			triangleResult[ntriang].a = vertlist[triTable[cubeindex][t]];
			triangleResult[ntriang].b = vertlist[triTable[cubeindex][t + 1]];
			triangleResult[ntriang].c = vertlist[triTable[cubeindex][t + 2]];
			ntriang++;
		}
		return(ntriang);
	}

	
	void MarchingCubes::cellImagePatch(size_t x, size_t y, size_t z, CubeMap::Face face)
	{
		std::string filename = "output/patches/";
		glm::vec3 direction(0.0f), position(0.0f);
		glm::vec3 samplePoint = glm::vec3(
			(((float(x) / (float)(scalarField.sizeX))) * 2.0f - 1.0f) * gridSpacing,
			(((float(y) / (float)(scalarField.sizeY))) * 2.0f - 1.0f) * gridSpacing,
			(((float(z) / (float)(scalarField.sizeZ))) * 2.0f - 1.0f) * gridSpacing);

		glm::vec3 nextSamplePoint = glm::vec3(
			(((float(x + 1) / (float)(scalarField.sizeX))) * 2.0f - 1.0f) * gridSpacing,
			(((float(y + 1) / (float)(scalarField.sizeY))) * 2.0f - 1.0f) * gridSpacing,
			(((float(z + 1) / (float)(scalarField.sizeZ))) * 2.0f - 1.0f) * gridSpacing);

		glm::vec3 previousSamplePoint = glm::vec3(
			(((float(x - 1) / (float)(scalarField.sizeX))) * 2.0f - 1.0f) * gridSpacing,
			(((float(y - 1) / (float)(scalarField.sizeY))) * 2.0f - 1.0f) * gridSpacing,
			(((float(z - 1) / (float)(scalarField.sizeZ))) * 2.0f - 1.0f) * gridSpacing);

		switch (face)
		{
		case CubeMap::POSITIVE_X:
			direction = glm::vec3(1.0f, 0.0f, 0.0f);
			position = glm::vec3(previousSamplePoint.x, samplePoint.y, samplePoint.z);
			filename += "rightWall_" + std::to_string(nPatches++) + "_.png";
			break;

		case CubeMap::NEGATIVE_X:
			direction = glm::vec3(-1.0f, 0.0f, 0.0f);
			filename += "leftWall_" + std::to_string(nPatches++) + "_.png";
			position = glm::vec3(nextSamplePoint.x, samplePoint.y, samplePoint.z);
			break;

		case CubeMap::POSITIVE_Y:
			direction = glm::vec3(0.0f, 1.0f, 0.0f);
			filename += "ceiling_" + std::to_string(nPatches++) + "_.png";
			position = glm::vec3(samplePoint.x, previousSamplePoint.y, samplePoint.z);
			break;

		case CubeMap::NEGATIVE_Y:
			direction = glm::vec3(0.0f, -1.0f, 0.0f);
			filename += "floor_" + std::to_string(nPatches++) + "_.png";
			position = glm::vec3(samplePoint.x, nextSamplePoint.y, samplePoint.z);
			break;
		
		case CubeMap::POSITIVE_Z:
			direction = glm::vec3(0.0f, 0.0f, 1.0f);
			filename += "frontWall_" + std::to_string(nPatches++) + "_.png";
			position = glm::vec3(samplePoint.x, samplePoint.y, previousSamplePoint.z);
			break;

		case CubeMap::NEGATIVE_Z:
			direction = glm::vec3(0.0f, 0.0f, -1.0f);
			filename += "backWall_" + std::to_string(nPatches++) + "_.png";
			position = glm::vec3(samplePoint.x, samplePoint.y, nextSamplePoint.z);
			break;

		default:
			assert(false);
			break;
		}

		//glm::vec3 pos = (samplePoint + nextSamplePoint) / 2.0f;
		cellRenderer.setCameraPosition(position);
		cellRenderer.setCameraTarget(direction);
		cellRenderer.setOrthoVolume(previousSamplePoint.x, nextSamplePoint.x, previousSamplePoint.y, nextSamplePoint.y, -1.0f, 100.0f);
		cellRenderer.update();
		cellRenderer.render();
		cellRenderer.writeImage(filename);
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
			return { i,     j,     k     };
		case 1:
			return { i + 1, j,     k     };
		case 2:
			return { i + 1, j,     k + 1 };
		case 3:
			return { i,     j,     k + 1 };
		case 4:
			return { i,     j + 1, k     };
		case 5:
			return { i + 1, j + 1, k     };
		case 6:
			return { i + 1, j + 1, k + 1 };
		case 7:
			return { i,     j + 1, k + 1 };
		default:
			return {};//shouldn't ever get here, polygonise shouldn't call for vertexIndex outside 0 to 7 - safer to put an assert here...
		}
	}
	Point3D MarchingCubes::interpolateVertex(double isoLevel, const std::array<size_t, 3>& xyzVertexA, const std::array<size_t, 3>& xyzVertexB)
	{
		Point3D& p1 = cubeLattice[xyzVertexA];
		Point3D& p2 = cubeLattice[xyzVertexB];

		Cell& cellA = scalarField[xyzVertexA], cellB = scalarField[xyzVertexB];

		double valp1 = (double)cellA.value;
		double valp2 = (double)cellB.value;

		if (cellA.mesh) {
			//UNDA_LOG_MESSAGE(cellA.mesh->vertices->size());
			//__debugbreak();
		}

		if (abs(isoLevel - valp1) < 0.00001)
			return(p1);
		if (abs(isoLevel - valp2) < 0.00001)
			return(p2);
		if (abs(valp1 - valp2) < 0.00001)
			return(p1);
		double mu = (isoLevel - valp1) / (valp2 - valp1);
		Point3D p;
		p.x = float(p1.x + (float)mu * (p2.x - p1.x));
		p.y = float(p1.y + (float)mu * (p2.y - p1.y));
		p.z = float(p1.z + (float)mu * (p2.z - p1.z));

		return(p);
	}

	CellRenderer::CellRenderer(Model* _model) :
		shaderProgram("resources/shaders/ortho_vertex_shader.glsl", "resources/shaders/ortho_fragment_shader.glsl"),
		frameBuffer(128, 128),
		model(_model)
	{
	}

	void CellRenderer::render()
	{
		frameBuffer.bind();
		shaderProgram.attach();
		GLCALL(glDisable(GL_CULL_FACE));

		unda::render::prepare(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		GLCALL(glUniformMatrix4fv(shaderProgram.getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view)));
		GLCALL(glUniformMatrix4fv(shaderProgram.getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection)));
		for (const Mesh& mesh : model->getMeshes()) {
			GLCALL(glBindVertexArray(mesh.vao));
				glm::mat4 transform = mesh.transform;
				//transform = glm::scale(transform, glm::vec3(1.0f / model->getModelScale()));
				//transform = glm::scale(transform, glm::vec3(2.0f));
				//transform = glm::translate(transform, glm::vec3(- (float)model->getModelScale() / 2.0f));

				GLCALL(glUniformMatrix4fv(shaderProgram.getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(transform)));
				GLCALL(glActiveTexture(GL_TEXTURE0));
				GLCALL(glUniform1i(shaderProgram.getUniformLocation("textureSampler"), 0));
				GLCALL(glBindTexture(GL_TEXTURE_2D, mesh.texture->getTextureId()));
				if (mesh.indices->size() > 0)
				{
					GLCALL(glDrawElements(GL_TRIANGLES, mesh.indices->size(), GL_UNSIGNED_INT, nullptr));
				}
				else {
					GLCALL(glDrawArrays(GL_TRIANGLES, 0, mesh.vertices->size()));
				}
		}
				GLCALL(glBindTexture(GL_TEXTURE_2D, NULL));
			GLCALL(glBindVertexArray(NULL));
		GLCALL(glEnable(GL_CULL_FACE));
		shaderProgram.detach();
		frameBuffer.unbind();
	}

	void CellRenderer::writeImage(const std::string& fileName)
	{
		unsigned char* image = frameBuffer.getImage();
		stbi_flip_vertically_on_write(1);
		int written = stbi_write_png(fileName.c_str(), frameBuffer.getWidth(), frameBuffer.getHeight(), STBI_rgb, (void*)image, frameBuffer.getWidth() * STBI_rgb * sizeof(unsigned char));
		delete[] image;
		if (!written) UNDA_ERROR("Image write failure!");
	}
}