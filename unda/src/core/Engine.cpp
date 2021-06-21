#include "Engine.h"


unda::Engine::Engine() 
	: application(new unda::GLFWApplication())
{
	scene = new unda::Scene();

	lightRenderer = new LightRenderer();
}

unda::Engine::Engine(int width, int height)
	: application(new unda::GLFWApplication())
{
	unda::windowWidth = width;
	unda::windowHeight = height;

	scene = new unda::Scene();

	lightRenderer = new LightRenderer();
}

unda::Engine::~Engine() {


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
	unda::render::prepare(glm::vec4(0.7f));
	scene->render();
	lightRenderer->drawLights(scene);

	application->processEvents();
}

void unda::Engine::cleanUp()
{

	delete scene;

	delete lightRenderer;
	delete application;
}

