#pragma once
#include "../rendering/RenderTools.h"
#include "../rendering/Texture.h"
#include "../utils/Settings.h"
#include "Primitives.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//class Texture;

namespace unda {

	class Model : public unda::Transform {
	public:
		Model() = default;
		Model(unsigned int vboLocation, unsigned int iboLocation, unsigned int idxCount, Texture* t = nullptr) { 
			vbo = vboLocation;
			ibo = iboLocation;
            indexCount = idxCount;
			texture = t;
		}
		Model(const std::vector<Vertex>& vertices, const std::vector<unsigned int> indices = std::vector<unsigned int>(), Texture* texture = nullptr);
		virtual ~Model();

		unsigned int getVBO() { return vbo; }
        unsigned int getIBO() { return ibo; }
		unsigned int getVAO() { return vao; }
        unsigned int getIndexCount() { return indexCount; }
		Texture* getTexture() { return texture; }

		bool hasIndexArray() { return hasIndices; }
		unsigned int getVertexCount() { return vertexCount; }
	private:
		unsigned int vao, vbo, ibo, indexCount = 0, vertexCount = 0;
		bool hasIndices = false;
		Texture* texture;
	};
	Model* loadModel(const std::string& modelPath, const std::string& texturePath = std::string());
	Model* createSphereModel(int resolution, float radius);

}