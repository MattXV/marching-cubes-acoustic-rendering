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
#include <../glob/glob.h>


//class Texture;

namespace unda {

	struct Mesh {
		Mesh(std::vector<Vertex>&& _vertices, std::vector<unsigned int>&& _indices, Texture* _texture, const std::string& _name = std::string())
			: vertices(std::move(_vertices))
			, indices(std::move(_indices))
			, texture(std::move(_texture))
			, name(_name)
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
		std::pair<glm::vec3, glm::vec3> aabb;
		glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f);

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
		void addMesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, Texture* texture, const std::string& name = std::string());

		void calculateAABB();
		void normaliseMeshes();
		void computeScalarField(std::array<float, 30 * 30 * 30>& scalarField);

	private:
		bool isBuffered = false;
		std::vector<Mesh> meshes;

	};
	Model* loadModel(const std::string& modelPath, Colour<float> baseColour = Colour<float>(0.7f, 0.7f, 0.7f, 1.0f), bool verbose = true);
	Model* createSphereModel(int resolution, float radius);
	Model* loadMeshDirectory(const std::string& directoryPath, const std::string& extension = "obj", const Colour<float>& baseColour = Colour<float>(1.0f, 1.0f, 1.0f, 1.0f), bool verbose = false);
}