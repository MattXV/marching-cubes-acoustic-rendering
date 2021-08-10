#pragma once

#include "../rendering/Renderer.h"
#include "../input/GLFWApplication.h"
#include "../scene/Scene.h"
#include "../rendering/LightRenderer.h"
#include <assert.h>
#if UNDA_TEST_SCENE == 1
	#include "../scene/TestScene.h"
#endif

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
#if UNDA_TEST_SCENE == 1
		unda::Scene* testScene;
#endif
		LightRenderer* lightRenderer;
		GLFWApplication* application;

	};

}