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
		BoundingBoxRenderer(unda::Camera* _camera);
		~BoundingBoxRenderer();

		void render();
		void cleanUp();
		inline void setCamera(unda::Camera* _camera) { camera = _camera; }

	private:
		glm::vec3 position = glm::vec3(0.0f, 20.0f, 0.0f);
		Shader shaderProgram;
		unsigned int VAO, VBO, UVBO;
		Camera* camera = nullptr;
		DISABLE_COPY_ASSIGN(BoundingBoxRenderer)
	};


	class FrameBuffer {
	public:
		FrameBuffer(int _width, int _height);
		~FrameBuffer();

		unsigned int getTextureLocation() { return textureLocation; }
		inline void bind() { GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, location)); GLCALL(glViewport(0, 0, width, height));	}
		inline void unbind() { GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, NULL)); GLCALL(glViewport(0, 0, windowWidth, windowHeight)); }

		unsigned char* getImage();
		int getWidth() { return width; }
		int getHeight() { return height; }

	private:
		unsigned int location = 0, textureLocation = 0, depthRenderBuffer = 0;
		unsigned int VAO = 0, IBO = 0, VBO = 0;
		int width, height;
		DISABLE_COPY_ASSIGN(FrameBuffer)
	};

	class GUIImage {
	public:
		GUIImage(float _width, float _height);
		~GUIImage();
		void render();
		void setTexture(unsigned int newTexture) { textureLocation = newTexture; }
	private:
		glm::mat4 projection = glm::ortho<float>(0.0f, windowWidth, windowHeight, 0.0f);
		Shader shaderProgram;
		float width, height;
		unsigned int VBO = 0, VAO = 0, textureLocation = 0;
		DISABLE_COPY_ASSIGN(GUIImage)
	};


}