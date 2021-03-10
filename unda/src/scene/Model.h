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
	struct Mesh {
		Mesh(const unsigned int& _vao, const unsigned int& _vbo, const unsigned int& _ibo,
			const long unsigned int& _vertexCount, const long unsigned int& _indexCount, Texture* t) 
			: texture(t)
		{
			vao = _vao;
			vbo = _vbo;
			ibo = _ibo;
			vertexCount = _vertexCount;
			indexCount = _indexCount;
		}
		unsigned int vao, vbo, ibo;
		long unsigned int vertexCount = 0, indexCount = 0;
		std::unique_ptr<Texture> texture;
	};


	class Model : public unda::Transform {
	public:
		Model() = default;
		Model(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Texture* texture = nullptr);
		virtual ~Model();

		std::vector<Mesh>& getMeshes() { return meshes; }
		const std::vector<Mesh>& getMeshes() const { return meshes; }
	private:
		void addMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Texture* texture);
		std::vector<Mesh> meshes;

	};
	Model* loadModel(const std::string& modelPath, const std::string& texturePath = std::string());
	Model* createSphereModel(int resolution, float radius);

}