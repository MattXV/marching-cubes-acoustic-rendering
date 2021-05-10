#include "Renderer.h"

namespace unda {
	void render::prepare(const glm::vec4& backgroundColour)
	{
		GLCALL(glClearColor(backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a));
		GLCALL(glEnable(GL_DEPTH_TEST));
		GLCALL(glDepthFunc(GL_LESS));
		GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	Shader::Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath) {
		programLocation = glCreateProgram();

		// Vertex Shader
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		std::string shaderSourceString = utils::ReadTextFile(vertexShaderFilePath);
		const char* sourceCString = shaderSourceString.c_str();
		glShaderSource(vertexShader, 1, (const char**)&sourceCString, nullptr);
		glCompileShader(vertexShader);
		int status = GL_FALSE;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE) {
			int maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
			char* errorLog = new char[maxLength];
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &errorLog[0]);
			glDeleteShader(vertexShader); // Don't leak the shader.
			UNDA_ERROR("[GLSL Error]: Vertex Shader");
			UNDA_ERROR(errorLog);
			delete[] errorLog;
			return;
		}
		glAttachShader(programLocation, vertexShader);

		// Fragment Shader
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		shaderSourceString = utils::ReadTextFile(fragmentShaderFilePath);
		sourceCString = shaderSourceString.c_str();
		glShaderSource(fragmentShader, 1, (const char**)&sourceCString, nullptr);
		glCompileShader(fragmentShader);
		status = GL_FALSE;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE) {
			int maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
			char* errorLog = new char[maxLength];
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &errorLog[0]);
			glDeleteShader(fragmentShader);
			UNDA_ERROR("[GLSL Error]: Fragment Shader");
			UNDA_ERROR(errorLog);
			delete[] errorLog;
			return;
		}
		glAttachShader(programLocation, fragmentShader);

		glLinkProgram(programLocation);
		glGetProgramiv(programLocation, GL_LINK_STATUS, &status);
		if (status != GL_TRUE) {
			UNDA_ERROR("Could not link shaders!");
		}
		glValidateProgram(programLocation);
		glGetProgramiv(programLocation, GL_VALIDATE_STATUS, &status);
		if (status != GL_TRUE) {
			UNDA_ERROR("Could not validate shaders!");
		}
		// Once successfully linked, shaders can be detached and deleted.
		GLCALL(glDetachShader(programLocation, vertexShader));
		GLCALL(glDetachShader(programLocation, fragmentShader));
		GLCALL(glDeleteShader(vertexShader));
		GLCALL(glDeleteShader(fragmentShader));
		GLCALL(glUseProgram(NULL));
	}
	
	BoundingBox::BoundingBox(const unda::AABB& aabb)
	: unda::AABB(aabb)
	{
		glm::vec3 min = glm::vec3(aabb.min.x, aabb.min.y, aabb.min.z);
		glm::vec3 max = glm::vec3(aabb.max.x, aabb.max.y, aabb.max.z);
		Point3D A_bottomleft{ min.x, min.y, min.z }, A_topleft{ min.x, max.y, min.z }, A_topright{ max.x, max.y, min.z }, A_bottomright{ max.x, min.y, min.z };
		Point3D B_bottomleft{ min.x, min.y, max.z }, B_topleft{ min.x, max.y, max.z }, B_topright{ max.x, max.y, max.z }, B_bottomright{ max.x, min.y, max.z };

		Triangle3D faces[6][2] = 
		{
			{ Triangle3D(A_topleft, A_bottomleft, A_topright), Triangle3D(A_topright, A_bottomleft, A_bottomright) },			// Front face
			{ Triangle3D(B_topleft, B_bottomleft, B_topright), Triangle3D(B_topright, B_bottomleft, B_bottomright) },			// Back face
			 
			{ Triangle3D(B_topleft, A_topleft, B_topright), Triangle3D(B_topright, A_topleft, A_topright) },					// Top face
			{ Triangle3D(B_bottomleft, A_bottomleft, B_bottomright), Triangle3D(B_bottomright, A_bottomleft, A_bottomright) },	// Bottom face
			 
			{ Triangle3D(A_topleft, A_bottomleft, B_bottomleft), Triangle3D(B_topleft, A_bottomleft, B_bottomleft) },			// Left face
			{ Triangle3D(A_topright, A_bottomright, B_topright), Triangle3D(B_topright, A_bottomright, B_bottomright) }			// Right face
		};

		for (size_t face = 0; face < 6; face++) {
			Triangle3D* cubeFace = faces[face];
			for (size_t triangle = 0; triangle < 2; triangle++) {
				Triangle3D triangleA = cubeFace[triangle];
				std::array<Vertex, 3> triangleVertices = triangleA.getVertices();
				for (size_t vertex = 0; vertex < 3; vertex++) {
					vertices[face * 2 * 3 + triangle * 3 + vertex] = triangleVertices[vertex];
				}
			}
		}

		

	}

	BoundingBoxRenderer::BoundingBoxRenderer(unda::Camera& cam)
		: shaders("resources/shaders/boundingbox_vertex_shader.glsl", "resources/shaders/boundingbox_fragment_shader.glsl")
		, camera(cam)
	{
	}

	BoundingBoxRenderer::~BoundingBoxRenderer()
	{
	}

	void BoundingBoxRenderer::render()
	{
		shaders.attach();
		for (std::unique_ptr<BoundingBox>& boundingBox : getBoundingBoxes()) {
			glm::mat4 model = unda::createModelMatrix(boundingBox->getRotation(), boundingBox->getPosition(), boundingBox->getScale());
			GLCALL(glUniformMatrix4fv(shaders.getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix())));
			GLCALL(glUniformMatrix4fv(shaders.getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(camera.getProjectionMatrix())));
			GLCALL(glUniformMatrix4fv(shaders.getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model)));

			//glDrawArrays(GL_TRIANGLES, 0, )
		}

		shaders.detach();
	}

	void BoundingBoxRenderer::cleanUp()
	{
	}

}