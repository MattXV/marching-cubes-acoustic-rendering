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
		glm::vec3 colour = glm::vec3(0.8f, 0.8f, 0.8f);
	};



	class Scene {
	public:
		Scene();
		virtual ~Scene();

		void update();

		// All temporary stuff. Sufficient for now.
		void addModel(unda::Model* newModel);
		void addLight(Light* newLight);
		const std::vector<Light*>& getLights() { return lights; };
		const std::vector<Model*>& getModels() { return models; };
		Camera* getCamera() { return camera; };

	private:
		std::vector<Model*> models;
		std::vector<Light*> lights;
		FPSCamera* camera;
	};
}
