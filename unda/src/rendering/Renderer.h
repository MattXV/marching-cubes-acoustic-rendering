#pragma once

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


	class BoundingBoxRenderer : public IBoundingBoxRenderer {
	public:
		BoundingBoxRenderer(unda::Camera& cam);
		~BoundingBoxRenderer();

		void render();
		void cleanUp();
	private:
		std::array<Vertex, 12> vertices;
		Shader shaders;
		Camera& camera;
		DISABLE_COPY_ASSIGN(BoundingBoxRenderer)
	};

}