#include "LightRenderer.h"

unda::LightRenderer::LightRenderer()
{
	std::string vertexSource   = unda::utils::ReadTextFile(unda::shaders::lightVertexShaderSource);
	std::string fragmentSource = unda::utils::ReadTextFile(unda::shaders::lightFragmentShaderSource);
	
	programId = glCreateProgram();
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* shaderSource = (const char*)vertexSource.c_str();
	glShaderSource(vertexShader, 1, &shaderSource, NULL);
	glCompileShader(vertexShader);
	int status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		unda::utils::printShaderError(vertexShader);
	}
	glAttachShader(programId, vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	shaderSource = (const char*)fragmentSource.c_str();
	glShaderSource(fragmentShader, 1, &shaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
		unda::utils::printShaderError(fragmentShader);
	glAttachShader(programId, fragmentShader);

	glLinkProgram(programId);
	glGetProgramiv(programId, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		std::cerr << "[GLSL Error]: Failed to link Light Shaders!" << std::endl;
	}
	glValidateProgram(programId);
	glGetProgramiv(programId, GL_VALIDATE_STATUS, &status);
	if (status != GL_TRUE) {
		std::cerr << "[GLSL Error]: Failed to validate Light Shaders!" << std::endl;
	}

	// Get Attributes
	vertexPositionLocation   = glGetAttribLocation(programId, "vertexPosition");
	// Uniforms
	modelMatrixLocation      = glGetUniformLocation(programId, "modelMatrix");
	viewMatrixLocation       = glGetUniformLocation(programId, "viewMatrix");
	projectionMatrixLocation = glGetUniformLocation(programId, "projectionMatrix");
	lightColourLocation = glGetUniformLocation(programId, "lightColour");
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

unda::LightRenderer::~LightRenderer()
{
	glDeleteProgram(programId);
}

void unda::LightRenderer::drawLights(Scene* scene)
{
	glUseProgram(programId);
	for (Light* light : scene->getLights()) {
		glm::mat4 modelMatrix = unda::createModelMatrix(light->getRotation(), light->getPosition(), light->getScale());
		GLCALL(glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix)));
		GLCALL(glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(scene->getCamera()->getViewMatrix())));
		GLCALL(glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(scene->getCamera()->getProjectionMatrix())));
		GLCALL(glUniform3fv(lightColourLocation, 1, glm::value_ptr(light->getColour())));

		GLCALL(glBindVertexArray(light->getVAO()));
		GLCALL(glDrawElements(GL_TRIANGLES, light->getIndexCount(), GL_UNSIGNED_INT, nullptr));
	}

	glUseProgram(NULL);
}
