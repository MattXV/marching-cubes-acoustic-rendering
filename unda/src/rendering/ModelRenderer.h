#pragma once

#include "Rendering.h"
#include "../utils/Utils.h"
#include "../utils/Settings.h"
#include "../scene/Scene.h"
#include "../scene/Camera.h"
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>


namespace unda { class Model; }


class ModelRenderer
{
public:
	ModelRenderer();
	~ModelRenderer();

	void drawModel(unda::Scene* scene);

private:
	unsigned int vertexArrayLocation;
	unsigned int programId;
	// Attributes
	static const int vertexPosition = 0, uvCoordinatesLayout = 1, vertexNormalLayout = 2;
	int vertexPositionLocation, uvCoordinatesLocation, vertexNormalLocation;
	// Uniforms
	int modelMatrixLocation, viewMatrixLocation, projectionMatrixLocation;
	int textureSamplerLocation, lightColourLocation, lightPositionLocation, viewPositionLocation;
	int normalSamplerLocation;
};