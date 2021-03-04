#pragma once
#include "../scene/Scene.h"
#include "../utils/Utils.h"
#include <glm/gtc/type_ptr.hpp>



namespace unda {
	class LightRenderer {
	public:
		LightRenderer();
		~LightRenderer();

		void drawLights(Scene* scene);
	private:
		unsigned int programId;

		// Attributes
		int vertexPositionLocation, vertexNormalLocation;
		// Uniforms
		int modelMatrixLocation, viewMatrixLocation, projectionMatrixLocation, lightColourLocation;
	};
}