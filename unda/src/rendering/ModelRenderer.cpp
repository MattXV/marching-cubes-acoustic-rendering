#include "ModelRenderer.h"

ModelRenderer::ModelRenderer()
{
	std::string vertexShaderSource = unda::utils::ReadTextFile("resources/shaders/vertex_shader.glsl");
	std::string fragmentShaderSource = unda::utils::ReadTextFile("resources/shaders/fragment_shader.glsl");

	programId = glCreateProgram();
	// Vertex Shader
	unsigned int vshader = glCreateShader(GL_VERTEX_SHADER);
	const char* shaderSource = vertexShaderSource.c_str();
	glShaderSource(vshader, 1, (const char**)&shaderSource, nullptr);
	glCompileShader(vshader);
	int status = GL_FALSE;
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		int maxLength = 0;
		glGetShaderiv(vshader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		char* errorLog =  new char[maxLength];
		glGetShaderInfoLog(vshader, maxLength, &maxLength, &errorLog[0]);

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(vshader); // Don't leak the shader.
		std::cout << "[GLSL Error]: " << errorLog << std::endl;
		std::cerr << "Could not compile vertex shader!" << std::endl;
		delete[] errorLog;
		return;
	}
	glAttachShader(programId, vshader);


	// Fragment Shader
	unsigned int fshader = glCreateShader(GL_FRAGMENT_SHADER);
	shaderSource = fragmentShaderSource.c_str();
	glShaderSource(fshader, 1, (const char**)&shaderSource, nullptr);
	glCompileShader(fshader);
	status = GL_FALSE;
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		int maxLength = 0;
		glGetShaderiv(fshader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		char* errorLog = new char[maxLength];
		glGetShaderInfoLog(fshader, maxLength, &maxLength, &errorLog[0]);

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		std::cout << "[GLSL Error]: " << errorLog << std::endl;
		glDeleteShader(fshader); // Don't leak the shader.
		std::cerr << "Could not compile fragment shader!" << std::endl;
		delete[] errorLog;
		return;
	}
	glAttachShader(programId, fshader);


	glLinkProgram(programId);
	glGetProgramiv(programId, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		std::cerr << "Could not link shaders!" << std::endl;
	}
	glValidateProgram(programId);
	glGetProgramiv(programId, GL_VALIDATE_STATUS, &status);
	if (status != GL_TRUE) {
		std::cerr << "Could not link shaders!" << std::endl;
	}
	
	// Vertex Shader attributes and uniforms
	vertexPositionLocation = glGetAttribLocation(programId, "vertexPosition");
	uvCoordinatesLocation = glGetAttribLocation(programId, "uvCoordinates");
	vertexNormalLocation = glGetAttribLocation(programId, "vertexNormal");

	modelMatrixLocation = glGetUniformLocation(programId, "modelMatrix");
	viewMatrixLocation = glGetUniformLocation(programId, "viewMatrix");
	projectionMatrixLocation = glGetUniformLocation(programId, "projectionMatrix");
	// Fragment Shader attributes and uniforms
	textureSamplerLocation = glGetUniformLocation(programId, "textureSampler");
	lightColourLocation = glGetUniformLocation(programId, "lightColour");
	lightPositionLocation = glGetUniformLocation(programId, "lightPosition");
	viewPositionLocation = glGetUniformLocation(programId, "viewPosition");

	glDeleteShader(vshader);
	glDeleteShader(fshader);
}

void ModelRenderer::drawModel(Model* model)
{
	glUseProgram(programId);

	//glBindBuffer(GL_ARRAY_BUFFER,)
	// Vertex Array

	glUseProgram(NULL);
}
