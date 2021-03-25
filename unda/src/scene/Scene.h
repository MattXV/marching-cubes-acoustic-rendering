#pragma once

#include <vector>
#include <string>
#include <functional>
#include "Model.h"
#include "Primitives.h"
#include "../utils/Settings.h"
#include "../utils/Utils.h"
#include "Camera.h"
#include "../input/Input.h"
//#include "../rendering/MarchingCubes.hpp"
#include "../../externals/happly/happly.h"
#include "Terrain.h"
#include "../rendering/VectorMarchingCubes.h"
#include "../acoustics/ImageSource.h"
#include <sndfile.h>



namespace unda {

	class Light : public unda::Transform {
	public:
		Light(const std::vector<Vertex>& vertices, const std::vector<unsigned int> indices = std::vector<unsigned int>());
		virtual ~Light();
		const glm::vec3& getColour() { return colour; }

		unsigned int getVAO() { return vao; }
		unsigned int getIndexCount() { return indexCount; }
	private:
		unsigned int vao, vbo, ibo, indexCount;
		bool hasIndices = false;
		glm::vec3 colour = glm::vec3(1.0f, 1.0f, 1.0f);
	};



	class Scene {
	public:
		Scene();
		virtual ~Scene();

		void update();

		// All temporary stuff. Sufficient for now.
		void addModel(unda::Model* newModel);
		void addModel(std::shared_ptr<Model>& newModel);
		void addLight(Light* newLight);
		const std::vector<Light*>& getLights() { return lights; };
		const std::vector<std::shared_ptr<Model>>& getModels() { return models; };
		Camera* getCamera() { return camera; };

	private:
		std::vector<std::shared_ptr<Model>> models;
		std::vector<Light*> lights;
		FPSCamera* camera;
	};

	//template<typename T, size_t sizeX, size_t sizeY, size_t sizeZ>
	//void computeScalarFieldFromMeshes(LatticeData3D<T, sizeX, sizeY, sizeZ>& data, Model* model)
	//{
	//	for (size_t x = 0; x < sizeX; ++x)
	//	{
	//		for (size_t y = 0; y < sizeY; ++y)
	//		{
	//			for (size_t z = 0; z < sizeZ; ++z)
	//			{
	//				glm::vec3 samplePoint = glm::vec3((float(x) / (float)sizeX) * 2.0f - 1.0f,
	//												  (float(y) / (float)sizeY) * 2.0f - 1.0f,
	//												  (float(z) / (float)sizeZ) * 2.0f - 1.0f);

	//				float fieldValue = 0.0f;
	//				for (Mesh& mesh : model->getMeshes()) {
	//					if (pointMeshCollision(samplePoint, mesh.aabb)) {
	//						fieldValue = 1.0f;
	//						break;
	//					}

	//				}
	//				data[std::array<size_t, 3>{x, y, z}] = fieldValue;
	//			}
	//		}
	//	}
	//}



}
