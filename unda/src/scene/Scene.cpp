#include "Scene.h"

unda::Scene::Scene()
{
	light = new Light(unda::primitives::createSphere(16, 1.0f));
	light->setPosition(glm::vec3(0.0f, 0.5f, 0.0f));
	
	camera = new unda::FPSCamera(90.0f, (float)unda::windowWidth / (float)windowHeight, 0.1f, 90.0f);
	//camera->setTarget(glm::vec3(1, 0, 1));
	//camera->setRotation(glm::vec3(30, 0, 0));

	model = unda::loadModel("resources/models/dmt_conference.obj", "resources/models/dmt_conference_texture.png");
	model->setPosition(glm::vec3(0.0f, 0.0f, 0.5f));
}

unda::Scene::~Scene()
{
	if (model) delete model;
	delete light;
	delete camera;
}

void unda::Scene::update()
{
	camera->handleInput();
}

void unda::Scene::addModel(unda::Model* newModel)
{
	model = newModel;
}
