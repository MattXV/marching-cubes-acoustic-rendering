#pragma once

#include "../rendering/RenderTools.h"
#include "../rendering/Texture.h"
#include "../utils/Settings.h"
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/Exporter.hpp>
#include <assimp/postprocess.h>
#include <vector>
#include <array>
#include <string>
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <json.hpp>
//class Texture;


namespace unda {
	struct LoadedMesh {
		LoadedMesh() = default;
		~LoadedMesh() = default;
		LoadedMesh(const LoadedMesh& other) { vertices.reset(other.vertices.get()); indices.reset(other.indices.get()); texture = other.texture; vao = other.vao; vbo = other.vbo; ibo = other.ibo; }
		LoadedMesh& operator=(const LoadedMesh& other) { vertices.reset(other.vertices.get()); indices.reset(other.indices.get()); texture = other.texture; vao = other.vao; vbo = other.vbo; ibo = other.ibo; return *this; }

		std::shared_ptr<std::vector<Vertex>> vertices = std::shared_ptr<std::vector<Vertex>>();
		std::shared_ptr<std::vector<unsigned int>> indices = std::shared_ptr<std::vector<unsigned int>>();
		Texture* texture = nullptr;
		unsigned int vao = 0, vbo = 0, ibo = 0;
	};
	LoadedMesh* loadMesh(const std::vector<Vertex>&& _vertices, const std::vector<unsigned int>&& _indices, Texture* texture);

	extern std::unordered_map<std::string, std::unique_ptr<Texture>> loadedTextures;
	extern std::unordered_map<std::string, std::vector<LoadedMesh>> loadedMeshes;
	
	// Mesh      -> VAO - VBO - IBO

	struct Mesh {
		Mesh(std::vector<Vertex>& _vertices, std::vector<unsigned int>& _indices, Texture* _texture, const std::string& _name = std::string(), glm::mat4 _transform = glm::mat4())
			: vertices(&_vertices)
			, indices(&_indices)
			, texture(_texture)
			, name(_name)
			, transform(_transform)
		{ 
			vertexCount = (long unsigned int)_vertices.size();
			indexCount = (long unsigned int)_indices.size();
		}
		Mesh() : vertices(nullptr), indices(nullptr) {}
		unsigned int vao = 0, vbo = 0, ibo = 0;
		long unsigned int vertexCount = 0, indexCount = 0;
		Texture* texture;
		//float scale = 1.0;
		glm::vec3 size = glm::vec3(0, 0, 0);
		AABB aabb;

		// Temporary CPU allocation
		std::vector<Vertex>* vertices;
		std::vector<unsigned int>* indices;

		// Transform
		glm::mat4 transform = glm::mat4(1.0f);
		glm::vec3 position = glm::vec3(), scale = glm::vec3(1.0f, 1.0f, 1.0f), rotation = glm::vec3();

		// Misc
		std::string name;
		std::string meshFileName;
		bool patchWritten = false;
	};

	class Model : public unda::Transform {
	public:
		Model() = default;
		~Model();


		std::vector<Mesh>& getMeshes() { return meshes; }
		const std::vector<Mesh>& getMeshes() const { return meshes; }

		double getModelScale() { return normalisationScale; }
		glm::vec3 getVolume() { return volume; }

		bool exportModel(const std::string& filename, bool overwrite = false);
		void recomputeAABBs() { normaliseMeshes(); calculateAABB(); }

		void calculateAABB();
		double normaliseMeshes();
		glm::vec3 calculateBoundingVolume();

		static unsigned int uniqueId;
	private:

		glm::vec3 volume = glm::vec3();
		double normalisationScale = 1.0;
		bool isBuffered = false;
		std::vector<Mesh> meshes;

	};

	Model* fromVertexData(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, const std::string& name, Texture* texture = nullptr);
	std::vector<Mesh> loadMeshes(const std::string& objFileName);
	Model* loadSceneGraph(const std::string& sceneGraphJson);
}