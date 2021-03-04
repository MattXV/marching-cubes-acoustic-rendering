#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include "../utils/Utils.h"
#include <glm/gtc/type_ptr.hpp>
#include "../scene/Scene.h"
#include "../scene/Camera.h"


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
};