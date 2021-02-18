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

class Model {
public:
	Model() = default;
	Model(unsigned int vboLocation, unsigned int iboLocation, Texture* t = nullptr) { 
		vbo = vboLocation;
		ibo = iboLocation;
		texture = t;
		position = glm::vec3(0, 0, 0);
		rotation = glm::vec3(0, 0, 0);
		scale = glm::vec3(1, 1, 1);
	}
	virtual ~Model() = default;

	unsigned int getVBO() { return vbo; }
	unsigned int getIBO() { return ibo; }
	Texture* getTexture() { return texture; }
	const glm::vec3 getPosition() { return position; }

private:
	unsigned int vbo, ibo;
	Texture* texture;
	glm::vec3 position, rotation, scale;
};


static Model* loadModel(const std::string& modelPath, const std::string& texturePath = std::string());