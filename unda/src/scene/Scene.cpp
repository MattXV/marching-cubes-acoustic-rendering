#include "Scene.h"

Scene::Scene(ModelRenderer* mR) : modelRenderer(mR)
{
}

Scene::~Scene()
{
	for (int i = 0; i < models.size(); i++) {
		delete models[i].release();
	}
}


void Scene::update()
{

}

void Scene::draw()
{
	//std::for_each(models.begin(), models.end(), [&](std::unique_ptr<unda::Model> model) { modelRenderer->drawModel(model.get()); });


}

void Scene::loadModel(const std::string& modelPath)
{
	std::filesystem::path path(modelPath);
	if (!std::filesystem::exists(path))
		return;

	models.push_back(std::unique_ptr<unda::Model>(unda::loadModel(modelPath)));
}
