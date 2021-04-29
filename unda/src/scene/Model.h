#pragma once

#include "../rendering/RenderTools.h"
#include "../rendering/Texture.h"
#include "../utils/Settings.h"
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../glob/glob.h"
#include <vector>
#include <string>
#include <filesystem>
//class Texture;


namespace unda {



	struct Mesh {
		Mesh(std::vector<Vertex>&& _vertices, std::vector<unsigned int>&& _indices, Texture* _texture, const std::string& _name = std::string(), Texture* normal = nullptr)
			: vertices(std::move(_vertices))
			, indices(std::move(_indices))
			, texture(std::move(_texture))
			, name(_name)
			, normalMap(normal)
		{ }
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
		unsigned int vao = 0, vbo = 0, ibo = 0;
		long unsigned int vertexCount = 0, indexCount = 0;
		std::unique_ptr<Texture> texture;
		std::unique_ptr<Texture> normalMap;
		float scale = 1.0;
		glm::vec3 size = glm::vec3(0, 0, 0);
		AABB aabb;

		// Temporary CPU allocation
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		// Misc
		std::string name;
	};

	class Model : public unda::Transform {
	public:
		Model() = default;
		Model(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, Texture* texture = nullptr);
		virtual ~Model();

		void toVertexArray();
		std::vector<Mesh>& getMeshes() { return meshes; }
		const std::vector<Mesh>& getMeshes() const { return meshes; }
		void addMesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, Texture* texture, const std::string& name = std::string(), Texture* normal = nullptr);

		void calculateAABB();
		void normaliseMeshes();


	private:
		bool isBuffered = false;
		std::vector<Mesh> meshes;

	};
	Model* loadSingleMesh(const std::string& modelPath, const std::string texturePath, const std::string normalPath);
	Model* loadModel(const std::string& modelPath, Colour<float> baseColour = Colour<float>(0.7f, 0.7f, 0.7f, 1.0f), bool verbose = true);
	Model* loadMeshDirectory(const std::string& directoryPath, const std::string& extension = "obj", const Colour<float>& baseColour = Colour<float>(1.0f, 1.0f, 1.0f, 1.0f), bool verbose = false);
}