#pragma once

#include "Renderer.h"
#include "../scene/Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


class ModelRenderer
{
public:
	ModelRenderer();
	~ModelRenderer() = default;

	void render(bool wireframe = false);
	void setModel(unda::Model* newModel) { model = newModel; }
	void setCamera(unda::Camera* newCamera) { camera = newCamera; }
	void setLightPosition(const glm::vec3& newPosition) { lightPosition = newPosition; }
	void setLightColour(const glm::vec3& newColour) { lightColour = newColour; }

private:
	unda::Camera* camera = nullptr;
	glm::vec3 lightColour = glm::vec3(1.0f, 1.0f, 1.0f), lightPosition = glm::vec3(1.0f, 1.0f, 1.0f);
	unda::Model* model = nullptr;
	unda::Shader shaders;
};