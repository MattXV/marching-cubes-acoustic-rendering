#pragma once

#include "Model.h"
#include "Primitives.h"
#include "../utils/Settings.h"
#include "../utils/Utils.h"

#include "Camera.h"
#include "../input/Input.h"
#include "../../externals/happly/happly.h"
#include "Terrain.h"
#include "../rendering/VectorMarchingCubes.h"
#include "../acoustics/AcousticSpace.h"
#include "../acoustics/DSP.h"

#include <FFTConvolver.h>
#include <vector>
#include <string>

#include <functional>


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
		const std::unordered_map<std::string, Model*>& getBoundingBoxes() const { return boundingBoxes; }

	private:
		std::unordered_map<std::string, Model*> boundingBoxes;
		std::vector<std::unique_ptr<Model>> boundingBoxesModels;

		unda::acoustics::AcousticSpace* acousticSpace;
		std::vector<std::shared_ptr<Model>> models;
		std::vector<Light*> lights;
		FPSCamera* camera;
	};


}
