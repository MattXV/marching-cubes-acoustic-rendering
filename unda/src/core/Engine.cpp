#include "Engine.h"




Engine::Engine() 
	: application(new GLFWApplication())
{
	if (!init()) return;
}

Engine::Engine(int width, int height)
	: application(new GLFWApplication())
{
	unda::windowWidth = width;
	unda::windowHeight = height;

	if (!init()) return;
	camera = nullptr;
	modelRenderer = new ModelRenderer();
}

Engine::~Engine() {
	delete camera;
	delete modelRenderer;
	delete application;
}

bool Engine::keepRunning()
{

	while (application->isRunning())
		return true;
	return false;
}

void Engine::update()
{
}

void Engine::render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	////glEnable(GL_CULL_FACE);

	application->processEvents();
}

bool Engine::init()
{

	return true;
}
