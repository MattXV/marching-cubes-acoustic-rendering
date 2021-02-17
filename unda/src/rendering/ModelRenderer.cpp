#include "ModelRenderer.h"

ModelRenderer::ModelRenderer()
{
	//char* vertexShader = Utils::readFile("resources/shaders/vertex_shader.glsl");
	//char* fragShader = Utils::readFile("resources/shaders/fragment_shader.glsl");
	//char* vertexShader = nullptr;
	//char* fragShader = nullptr;

	//programId = glCreateProgram();
	//// Vertex Shader
	//unsigned int vshader = glCreateShader(GL_VERTEX_SHADER);
	//glShaderSource(vshader, 1, &vertexShader, nullptr);
	//glCompileShader(vshader);
	//int status = GL_FALSE;
	//glGetShaderiv(vshader, GL_COMPILE_STATUS, &status);
	//if (status != GL_TRUE) {
	//	std::cerr << "Could not compile vertex shader!" << std::endl;
	//}

	//// Fragment Shader
	//unsigned int fshader = glCreateShader(GL_FRAGMENT_SHADER);
	//glShaderSource(fshader, 1, &fragShader, nullptr);

	//glCompileShader(fshader);
	//status = GL_FALSE;
	//glGetShaderiv(fshader, GL_COMPILE_STATUS, &status);
	//if (status != GL_TRUE) {
	//	std::cerr << "Could not compile vertex shader!" << std::endl;
	//}

	//glLinkProgram(programId);
	//glGetProgramiv(programId, GL_LINK_STATUS, &status);
	//if (status != GL_TRUE) {
	//	std::cerr << "Could not lnik shaders!" << std::endl;
	//}
	//glValidateProgram(programId);
	//glGetProgramiv(programId, GL_VALIDATE_STATUS, &status);
	//if (status != GL_TRUE) {
	//	std::cerr << "Could not lnik shaders!" << std::endl;
	//}
	//
	//glDeleteShader(vshader);
	//glDeleteShader(fshader);
	//// Link shaders
	//if (vertexShader) delete vertexShader;
	//if (fragShader) delete fragShader;
}

void ModelRenderer::drawModel()
{
	//glUseProgram(programId);

	////GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 12));
	//
	//glUseProgram(NULL);
}
