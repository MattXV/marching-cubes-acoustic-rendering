#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include "../utils/Utils.h"
#include "../scene/Model.h"

namespace unda { class Model; }

class ModelRenderer
{
public:
	ModelRenderer();
	~ModelRenderer() = default;

	void drawModel(unda::Model* model);

private:
	unsigned int programId;
	// Attributes
	int vertexPositionLocation, uvCoordinatesLocation, vertexNormalLocation;
	// Uniforms
	int modelMatrixLocation, viewMatrixLocation, projectionMatrixLocation;
	int textureSamplerLocation, lightColourLocation, lightPositionLocation, viewPositionLocation;
};