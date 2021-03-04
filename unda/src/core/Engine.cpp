#include "Engine.h"


unda::Engine::Engine() 
	: application(new unda::GLFWApplication())
{
	if (!init()) return;
	scene = new unda::Scene();
	modelRenderer = new ModelRenderer();
	lightRenderer = new LightRenderer();
}

unda::Engine::Engine(int width, int height)
	: application(new unda::GLFWApplication())
{
	unda::windowWidth = width;
	unda::windowHeight = height;

	if (!init()) return;
	scene = new unda::Scene();
	modelRenderer = new ModelRenderer();
}

unda::Engine::~Engine() {
	delete scene;
	delete modelRenderer;
	delete lightRenderer;
	delete application;
}

bool unda::Engine::keepRunning()
{
	while (application->isRunning())
		return true;
	return false;
}

void unda::Engine::update(double deltaTime)
{
	// TODO: Implement fixed timestep! 
	scene->update();
}

void unda::Engine::render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_CULL_FACE);
	lightRenderer->drawLights(scene);
	modelRenderer->drawModel(scene);

	application->processEvents();
}

bool unda::Engine::init()
{

	return true;
}
