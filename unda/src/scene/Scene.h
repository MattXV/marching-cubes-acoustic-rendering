#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include "Model.h"
#include "../rendering/ModelRenderer.h"


class Scene {
public:
	Scene(ModelRenderer* mR);
	virtual ~Scene();

	void update();
	void draw();
	
	void loadModel(const std::string& modelPath);

private:
	ModelRenderer* modelRenderer;
	std::vector<std::unique_ptr<unda::Model>> models;
};