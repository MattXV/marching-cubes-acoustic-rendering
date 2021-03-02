#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../rendering/ModelRenderer.h"
#include "../input/GLFWApplication.h"
#include <assert.h>
#include "../scene/Scene.h"


class GLFWApplication;
class ModelRenderer;

namespace unda {


	class Engine
	{
	public:
		Engine();
		Engine(int width, int height);
		~Engine();

		bool keepRunning();
		void update(double deltaTime);
		void render();
		void* getWindow() { return application->getWindow(); }
	private:
		unda::Scene* scene;
		ModelRenderer* modelRenderer;
		GLFWApplication* application;

		// Program
		bool init();

	};

}