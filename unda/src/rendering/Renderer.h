#pragma once

#include "Texture.h"
#include "../utils/Utils.h"
#include "../scene/SceneRenderer.h"
#include "../scene/Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>


namespace unda {
	namespace render {
		void prepare(const glm::vec4& backgroundColour = glm::vec4(0.33f, 0.33f, 0.33f, 1.0f));
	}

	class Shader {
	public:
		Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
		~Shader() { GLCALL(glDeleteProgram(programLocation)); }

		void attach() { GLCALL(glUseProgram(programLocation)); }
		void detach() { GLCALL(glUseProgram(NULL)); }
		int getUniformLocation(const std::string& uniform) { return glGetUniformLocation(programLocation, uniform.c_str()); }
	private:
		unsigned int programLocation = 0;
		DISABLE_COPY_ASSIGN(Shader)
	};

	class BoundingBox : public IBoundingBox {
	public:
		BoundingBox(const unda::AABB&);
		const std::array<Vertex, 36>& getVertices() { return vertices; }
		unsigned int getTextureLocation() { return cubeMap.getTextureLocation(); }
		void doPatch(TexturePatch& patch, CubeMap::Face face);
	private:
		CubeMap cubeMap;
		std::array<Vertex, 36> vertices;
		DISABLE_COPY_ASSIGN(BoundingBox)
	};

	class BoundingBoxRenderer : public IBoundingBoxRenderer {
	public:
		BoundingBoxRenderer(unda::Camera& cam);
		~BoundingBoxRenderer();

		void render();
		void cleanUp();

	private:
		glm::vec3 position = glm::vec3(20.0f, 20.0f, 20.0f);
		Shader shaderProgram;
		unsigned int VAO, VBO, UVBO;
		Camera& camera;
		DISABLE_COPY_ASSIGN(BoundingBoxRenderer)
	};

}