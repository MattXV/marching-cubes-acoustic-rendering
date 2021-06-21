#pragma once

#include "../rendering/Renderer.h"
#include "../input/GLFWApplication.h"
#include "../scene/Scene.h"
#include "../rendering/LightRenderer.h"
#include <assert.h>


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

		void cleanUp();
	private:
		unda::Scene* scene;
		LightRenderer* lightRenderer;
		GLFWApplication* application;

	};

}