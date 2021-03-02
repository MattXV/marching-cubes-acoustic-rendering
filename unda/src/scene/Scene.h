#pragma once

#include <vector>
#include <string>
#include <functional>
#include "Model.h"
#include "Primitives.h"
#include "../utils/Settings.h"
#include "Camera.h"
#include "../input/Input.h"


namespace unda {

	class Light : public unda::Transform {
	public:
		Light(unda::Model* m) { model = m; }
		virtual ~Light() { delete model; }
		const glm::vec3& getColour() { return colour; }

	private:
		glm::vec3 colour = glm::vec3(0.7f, 0.7f, 0.7f);
		unda::Model* model;
	};

	class Scene {
	public:
		Scene();
		virtual ~Scene();

		void update();

		// All temporary stuff. Sufficient for now.
		void addModel(unda::Model* newModel);
		Light* getLight() { return light; };
		unda::Model* getModel() { return model; };
		Camera* getCamera() { return camera; };

	private:
		FPSCamera* camera;
		unda::Model* model;
		Light* light;
	};
}
