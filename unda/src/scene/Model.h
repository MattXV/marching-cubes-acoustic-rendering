#pragma once
#include "../rendering/RenderTools.h"
#include "../rendering/Texture.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Texture;

class Model : public unda::Transform {
public:
	Model() = default;
	Model(unsigned int vboLocation, unsigned int iboLocation, Texture* t = nullptr) { 
		vbo = vboLocation;
		ibo = iboLocation;
		texture = t;
	}
	virtual ~Model() = default;

	unsigned int getVBO() { return vbo; }
	unsigned int getIBO() { return ibo; }
	Texture* getTexture() { return texture; }

private:
	unsigned int vbo, ibo;
	Texture* texture;
};


static Model* loadModel(const std::string& modelPath, const std::string& texturePath = std::string());