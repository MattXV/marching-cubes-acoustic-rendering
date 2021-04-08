#pragma once
#include "../rendering/RenderTools.h"
#include "Model.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>


namespace unda {



	namespace primitives {
		std::pair<std::vector<Vertex>, std::vector<unsigned int>> createSphere(int resolution, float radius);
		unda::Model* cubeBoundingBox(const AABB& aabb);
		Model* createSphereModel(int resolution, float radius);

	}
}