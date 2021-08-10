#include "Renderer.h"

namespace unda {
	std::vector<std::unique_ptr<IBoundingBox>> IBoundingBoxRenderer::boundingBoxes = std::vector<std::unique_ptr<IBoundingBox>>();

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
			UNDA_ERROR(errorLog);
			UNDA_ERROR("[GLSL Error]: Vertex Shader");
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
			UNDA_ERROR(errorLog);
			UNDA_ERROR("[GLSL Error]: Fragment Shader");
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
	
	BoundingBox::BoundingBox(const unda::AABB& aabb) : IBoundingBox(aabb), cubeMap()
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
			 
			{ Triangle3D(A_topleft, A_bottomleft, B_bottomleft), Triangle3D(A_topleft, A_bottomleft, B_topleft) },			// Left face
			{ Triangle3D(A_topright, A_bottomright, B_topright), Triangle3D(B_topright, A_bottomright, B_bottomright) }			// Right face
		};



		Vertex minBottomLeft(min.x, min.y, min.z, 0, 0, 0, 0, 0), minTopLeft(min.x, max.y, min.z, 0, 0, 0, 0, 0);
		Vertex minTopRight(max.x, max.y, min.z, 0, 0, 0, 0, 0), minBottomRight(max.x, min.y, min.z, 0, 0, 0, 0, 0 );
		
		Vertex maxBottomLeft(min.x, min.y, max.z, 0, 0, 0, 0, 0), maxTopLeft(min.x, max.y, max.z, 0, 0, 0, 0, 0);
		Vertex maxTopRight(max.x, max.y, max.z, 0, 0, 0, 0, 0), maxBottomRight(max.x, min.y, max.z, 0, 0, 0, 0, 0);



		vertices[0]  = minTopLeft;
		vertices[1]  = minBottomLeft;
		vertices[2]  = minTopRight;
		vertices[3]  = minTopRight;
		vertices[4]  = minBottomLeft;
		vertices[5]  = minBottomRight;
					 
		vertices[6]  = maxTopLeft;
		vertices[7]  = maxBottomLeft;
		vertices[8]  = maxTopRight;
		vertices[9]  = maxTopRight;
		vertices[10] = maxBottomLeft;
		vertices[11] = maxBottomRight;

		vertices[12] = maxBottomLeft;
		vertices[13] = maxTopLeft;
		vertices[14] = minTopLeft;
		vertices[15] = minTopLeft;
		vertices[16] = minBottomLeft;
		vertices[17] = maxBottomLeft;

		vertices[18] = maxBottomRight;
		vertices[19] = maxTopRight;
		vertices[20] = minTopRight;
		vertices[21] = minTopRight;
		vertices[22] = minBottomRight;
		vertices[23] = maxBottomRight;

		vertices[24] = maxTopLeft;
		vertices[25] = maxTopRight;
		vertices[26] = minTopLeft;
		vertices[27] = minTopLeft;
		vertices[28] = minTopRight;
		vertices[29] = maxTopRight;

		vertices[30] = maxBottomLeft;
		vertices[31] = maxBottomRight;
		vertices[32] = minBottomLeft;
		vertices[33] = minBottomLeft;
		vertices[34] = minBottomRight;
		vertices[35] = maxBottomRight;


		//for (size_t face = 0; face < 6; face++) {
		//	Triangle3D* cubeFace = faces[face];
		//	for (size_t triangle = 0; triangle < 2; triangle++) {
		//		Triangle3D triangleA = cubeFace[triangle];
		//		std::array<Vertex, 3> triangleVertices = triangleA.getVertices();
		//		for (size_t vertex = 0; vertex < 3; vertex++) {
		//			vertices[face * 2 * 3 + triangle * 3 + vertex] = triangleVertices[vertex];
		//		}
		//	}
		//}

	}

	void BoundingBox::doPatch(TexturePatch& patch, CubeMap::Face face)
	{
		cubeMap.applyPatch(patch, face);
	}

	BoundingBoxRenderer::BoundingBoxRenderer(unda::Camera* _camera)
		: shaderProgram("resources/shaders/boundingbox_vertex_shader.glsl", "resources/shaders/boundingbox_fragment_shader.glsl")
		, camera(_camera)
	{
		shaderProgram.attach();
		
		float UVs[6][2][3] = {
			{ {0, 1, 0}, { 1, 0, 0} },
			{  {0, 1, 1}, { 1, 0, 1} },
			{  {0, 1, 1}, { 1, 1, 0} },
			{  {0, 0, 1}, { 1, 0, 0} },
			{  {0, 0, 0}, { 0, 1, 1} },
			{  {1, 1, 0},  { 1, 0, 1} },
		};

		GLCALL(glGenVertexArrays(1, &VAO));
		GLCALL(glGenBuffers(1, &VBO));
		GLCALL(glGenBuffers(1, &UVBO));
		GLCALL(glBindVertexArray(VAO));
		
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
		
		GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 36, NULL, GL_DYNAMIC_DRAW));
		GLCALL(glEnableVertexAttribArray(0));
		GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr));

		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, UVBO));
		GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(UVs), UVs, GL_STATIC_DRAW));
		GLCALL(glEnableVertexAttribArray(1));
		GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr));

		GLCALL(glBindVertexArray(NULL));
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, NULL));
		shaderProgram.detach();
	}

	BoundingBoxRenderer::~BoundingBoxRenderer()
	{
		GLCALL(glDeleteVertexArrays(1, &VAO));
		GLCALL(glDeleteBuffers(1, &VBO));
		GLCALL(glDeleteBuffers(1, &UVBO));
	}

	void BoundingBoxRenderer::render()
	{
		shaderProgram.attach();
		glDisable(GL_CULL_FACE);
		//GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for (std::unique_ptr<IBoundingBox>& boundingBox : getBoundingBoxes()) {
			glm::mat4 model = unda::createModelMatrix(boundingBox->getRotation(), glm::vec3(0.0f), boundingBox->getScale());
			model = glm::translate(model, glm::vec3(3.0f, 3.0f, 3.0f));
			model = glm::translate(model, glm::vec3(boundingBox->min.x, boundingBox->min.y, boundingBox->min.z));
			model = glm::translate(model, position);
			GLCALL(glUniformMatrix4fv(shaderProgram.getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix())));
			GLCALL(glUniformMatrix4fv(shaderProgram.getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix())));
			GLCALL(glUniformMatrix4fv(shaderProgram.getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model)));

			//glm::vec3 cubeSize = glm::vec3(
			//	fabs(boundingBox->max.x - boundingBox->min.x),
			//	fabs(boundingBox->max.y - boundingBox->min.y), 
			//	fabs(boundingBox->max.z - boundingBox->min.z));
			glm::vec3 cubeSize = glm::vec3(1.0f, 1.0f, 1.0f);
			//glm::vec3 cubePosition = glm::vec3()

			GLCALL(glBindVertexArray(VAO));
				GLCALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
				GLCALL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * boundingBox->getVertices().size(), boundingBox->getVertices().data()));

				GLCALL(glActiveTexture(GL_TEXTURE0));
				GLCALL(glUniform1i(shaderProgram.getUniformLocation("cubeMap"), 0));
				//GLCALL(glUniform3fv(shaderProgram.getUniformLocation("cubeSize"), 1, glm::value_ptr(cubeSize)));
					GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, boundingBox->getTextureLocation()));

					GLCALL(glDrawArrays(GL_TRIANGLES, 0, boundingBox->getVertices().size()));

					GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, NULL));

				GLCALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
			GLCALL(glBindVertexArray(NULL));

		}
		glEnable(GL_CULL_FACE);
		//GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		shaderProgram.detach();
	}

	void BoundingBoxRenderer::cleanUp()
	{
	}


	FrameBuffer::FrameBuffer(int _width, int _height)
		: width(_width),
		  height(_height)
	{
		GLCALL(glGenFramebuffers(1, &location));
		GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, location));

		GLCALL(glGenTextures(1, &textureLocation));
		GLCALL(glBindTexture(GL_TEXTURE_2D, textureLocation));
		GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers);

		GLCALL(glGenRenderbuffers(1, &depthRenderBuffer));
		GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer));
		GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height));
		GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer));
		GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureLocation, 0));

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			UNDA_ERROR("Could not create a new framebuffer!");

		GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, location));
		GLCALL(glViewport(0, 0, width, height));

		GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, NULL));
		GLCALL(glBindTexture(GL_TEXTURE_2D, NULL));
		GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, NULL));
		GLCALL(glViewport(0, 0, windowWidth, windowHeight));
	}

	FrameBuffer::~FrameBuffer() {
		GLCALL(glDeleteFramebuffers(1, &location));
	}

	unsigned char* FrameBuffer::getImage()
	{
		unsigned char* image = new unsigned char[(size_t)width * (size_t)height * 3];
		GLCALL(glBindTexture(GL_TEXTURE_2D, textureLocation));
			GLCALL(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)image));
		GLCALL(glBindTexture(GL_TEXTURE_2D, NULL));
		return image;
	}



	GUIImage::GUIImage(float _width, float _height)
		: shaderProgram("resources/shaders/gui_vertex_shader.glsl", "resources/shaders/gui_fragment_shader.glsl"),
		width(_width),
		height(_height)
	{
		float quad[] = {
			0.0f,  0.0f,   0.0f, 0.0f,
			0.0f,  height, 0.0f, 1.0f,
			width, height, 1.0f, 1.0f,
			width, height, 1.0f, 1.0f,
			width, 0.0f,   1.0f, 0.0f,
			0.0f,  0.0f,   0.0f, 0.0f
		};

		GLCALL(glGenVertexArrays(1, &VAO));
		GLCALL(glBindVertexArray(VAO));
			GLCALL(glGenBuffers(1, &VBO));
			GLCALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
				GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW));
				GLCALL(glEnableVertexAttribArray(0));
				GLCALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr));
				GLCALL(glEnableVertexAttribArray(1));
				GLCALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2)));
		GLCALL(glBindVertexArray(NULL));
			GLCALL(glBindBuffer(GL_ARRAY_BUFFER, NULL));
	}

	GUIImage::~GUIImage()
	{
		GLCALL(glDeleteBuffers(1, &VBO));
		GLCALL(glDeleteVertexArrays(1, &VAO));
	}

	void GUIImage::render() {
		shaderProgram.attach();
		GLCALL(glBindVertexArray(VAO));

		GLCALL(glUniformMatrix4fv(shaderProgram.getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection)));
		GLCALL(glUniform1i(shaderProgram.getUniformLocation("imageTexture"), 0));
		GLCALL(glActiveTexture(GL_TEXTURE0));
		GLCALL(glBindTexture(GL_TEXTURE_2D, textureLocation));
		GLCALL(glDrawArrays(GL_TRIANGLES, 0, 2 * 3));

		GLCALL(glBindVertexArray(NULL));
		shaderProgram.detach();
	}
}