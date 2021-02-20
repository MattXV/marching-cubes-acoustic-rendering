#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../rendering/ModelRenderer.h"
#include "../input/GLFWApplication.h"
#include <math.h>
#include <iostream>
#include <assert.h>

#include "../scene/Camera.h"


class GLFWApplication;
class ModelRenderer;

class Engine
{
public:
	Engine();
	Engine(int width, int height);
	~Engine();

	bool keepRunning();
	void update();
	void render();

private:
	ModelRenderer* modelRenderer;
	GLFWApplication* application;
	unda::Camera* camera;

	// Program
	bool init();

};