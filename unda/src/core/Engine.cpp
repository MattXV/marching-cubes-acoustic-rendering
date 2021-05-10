#include "Engine.h"


unda::Engine::Engine() 
	: application(new unda::GLFWApplication())
{
	scene = new unda::Scene();
	modelRenderer = new ModelRenderer();
	lightRenderer = new LightRenderer();
}

unda::Engine::Engine(int width, int height)
	: application(new unda::GLFWApplication())
{
	unda::windowWidth = width;
	unda::windowHeight = height;

	scene = new unda::Scene();
	modelRenderer = new ModelRenderer();
	lightRenderer = new LightRenderer();
}

unda::Engine::~Engine() {

	delete scene;
	delete modelRenderer;
	delete lightRenderer;
	delete application;
}

bool unda::Engine::keepRunning()
{
	while (application->isRunning()) return true;
	cleanUp();
	return false;
}

void unda::Engine::update(double deltaTime)
{
	// TODO: Implement fixed timestep! 
	scene->update();
}

void unda::Engine::render()
{
	unda::render::prepare();
	scene->render();
	lightRenderer->drawLights(scene);
	modelRenderer->drawModel(scene);

	application->processEvents();
}

void unda::Engine::cleanUp()
{
	textures.clear();
}

