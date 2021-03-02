#pragma once

#include "../scene/Model.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>


namespace unda {
	namespace primitives {
		unda::Model* createSphere(int resolution, float radius);
	}
}