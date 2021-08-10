#pragma once

#include "../rendering/ModelRenderer.h"
#include "Model.h"
#include "Primitives.h"
#include "../utils/Settings.h"
#include "../utils/Utils.h"

#include "Camera.h"
#include "../input/Input.h"

#include "Terrain.h"
#include "../rendering/VectorMarchingCubes.h"
#include "../acoustics/ImageSource.h"
#include "../acoustics/DSP.h"

#include <vector>
#include <string>
#include <memory>
#include <filesystem>
#include <functional>
#include <json.hpp>

using json = nlohmann::json;

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

		virtual void update();
		virtual void render();
		// All temporary stuff. Sufficient for now.
		void addModel(unda::Model* newModel);
		void addModel(std::shared_ptr<Model>& newModel);
		void addLight(Light* newLight);
		const std::vector<Light*>& getLights() { return lights; };
		const std::vector<std::shared_ptr<Model>>& getModels() { return models; };

		Camera* getCamera() { return camera; };
		const std::unordered_map<std::string, Model*>& getBoundingBoxes() const { return boundingBoxes; }

	protected:
		virtual void init();
		std::shared_ptr<Model> inputScene, marchingCubesModel;

		std::unordered_map<std::string, Model*> boundingBoxes;
		std::vector<std::unique_ptr<Model>> boundingBoxesModels;


		std::vector<std::shared_ptr<Model>> models;
		std::vector<Light*> lights;
		Camera* camera;
		BoundingBoxRenderer boundingBoxRenderer;
		ModelRenderer modelRenderer;
	};


}
