#include "ModelRenderer.h"

ModelRenderer::ModelRenderer() 
{
	std::string vertexShaderSource = unda::utils::ReadTextFile(unda::shaders::vertexShaderSource);
	std::string fragmentShaderSource = unda::utils::ReadTextFile(unda::shaders::fragmentShaderSource);

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
	// Once successfully linked, shaders can be detached and deleted.
	glDetachShader(programId, vshader);
	glDetachShader(programId, fshader);
	glDeleteShader(vshader);
	glDeleteShader(fshader);

	// Vertex Shader attributes and uniforms
	vertexPositionLocation = glGetAttribLocation(programId, "vertexPosition");
	uvCoordinatesLocation = glGetAttribLocation(programId, "uvCoordinates");
	vertexNormalLocation = glGetAttribLocation(programId, "vertexNormal");

	modelMatrixLocation = glGetUniformLocation(programId, "modelMatrix");
	viewMatrixLocation = glGetUniformLocation(programId, "viewMatrix");
	projectionMatrixLocation = glGetUniformLocation(programId, "projectionMatrix");
	// Fragment Shader attributes and uniforms
	textureSamplerLocation = glGetUniformLocation(programId, "textureSampler");
	normalSamplerLocation = glGetUniformLocation(programId, "normal_map");
	lightColourLocation = glGetUniformLocation(programId, "lightColour");
	lightPositionLocation = glGetUniformLocation(programId, "lightPosition");
	viewPositionLocation = glGetUniformLocation(programId, "viewPosition");

	glGenVertexArrays(1, &vertexArrayLocation);
	glBindVertexArray(vertexArrayLocation);
	glUseProgram(NULL);
}

ModelRenderer::~ModelRenderer()
{
	glDeleteProgram(programId);
}

void ModelRenderer::drawModel(unda::Scene* scene)
{
	GLCALL(glUseProgram(programId));

	for (const std::shared_ptr<unda::Model>& model : scene->getModels()) {

			glm::mat4 modelMatrix = unda::createModelMatrix(model->getRotation(), model->getPosition(), model->getScale());
			GLCALL(glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix)));
			GLCALL(glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(scene->getCamera()->getViewMatrix())));
			GLCALL(glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(scene->getCamera()->getProjectionMatrix())));
			// Fragment shader uniforms
			GLCALL(glUniform3fv(lightColourLocation, 1, glm::value_ptr(scene->getLights()[0]->getColour())));
			GLCALL(glUniform3fv(lightPositionLocation, 1, glm::value_ptr(scene->getLights()[0]->getPosition())));
			GLCALL(glUniform3fv(viewPositionLocation, 1, glm::value_ptr(scene->getCamera()->getPosition())));
			// Bind texture

		// Vertex shader uniforms
		for (unda::Mesh& mesh : model->getMeshes()) {

			GLCALL(glBindVertexArray(mesh.vao));

			GLCALL(glActiveTexture(GL_TEXTURE0));
			GLCALL(glUniform1i(textureSamplerLocation, 0));
			GLCALL(glBindTexture(GL_TEXTURE_2D, mesh.texture->getTextureId()));

			Texture* normalMap = mesh.normalMap.get();
			if (normalMap) {
				GLCALL(glActiveTexture(GL_TEXTURE0 + 2));
				GLCALL(glUniform1i(normalSamplerLocation, 2));
				GLCALL(glBindTexture(GL_TEXTURE_2D, normalMap->getTextureId()));
			}

				GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

			if (mesh.indexCount > 0) {
				GLCALL(glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr));
			}
			else {
				GLCALL(glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount));
			}
				GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

			GLCALL(glBindTexture(GL_TEXTURE_2D, NULL));


			GLCALL(glBindVertexArray(NULL));
		}

	}

	// Bounding Boxes
	for (std::pair<std::string, unda::Model*> modelAABB : scene->getBoundingBoxes()) {
		for (unda::Mesh& mesh : modelAABB.second->getMeshes()) {
			GLCALL(glBindVertexArray(mesh.vao));
			
			//glm::mat4 modelMatrix = unda::createModelMatrix(modelAABB.second->getRotation(), modelAABB.second->getPosition(), modelAABB.second->getScale());
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			GLCALL(glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix)));
			GLCALL(glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(scene->getCamera()->getViewMatrix())));
			GLCALL(glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(scene->getCamera()->getProjectionMatrix())));
			// Fragment shader uniforms
			GLCALL(glUniform3fv(lightColourLocation, 1, glm::value_ptr(scene->getLights()[0]->getColour())));
			GLCALL(glUniform3fv(lightPositionLocation, 1, glm::value_ptr(scene->getLights()[0]->getPosition())));
			GLCALL(glUniform3fv(viewPositionLocation, 1, glm::value_ptr(scene->getCamera()->getPosition())));
			// Bind texture

			Texture* texture = mesh.texture.get();
			if (texture) {
				GLCALL(glActiveTexture(GL_TEXTURE0));
				GLCALL(glUniform1i(textureSamplerLocation, 0));
				GLCALL(glBindTexture(GL_TEXTURE_2D, mesh.texture->getTextureId()));


			}

			Texture* normalMap = mesh.normalMap.get();
			if (normalMap) {
				GLCALL(glActiveTexture(GL_TEXTURE0 + 2));
				GLCALL(glUniform1i(normalSamplerLocation, 2));
				GLCALL(glBindTexture(GL_TEXTURE_2D, normalMap->getTextureId()));
			}

			//GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene->getModel()->getIBO()));
			//GLCALL(glEnableVertexAttribArray(unda::shaders::vertexPositionLocation));
			//GLCALL(glEnableVertexAttribArray(unda::shaders::uvCoordinatesLocation));
			//GLCALL(glEnableVertexAttribArray(unda::shaders::vertexNormalLocation));

			if (mesh.indexCount > 0) {
				GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
				GLCALL(glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr));
				GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
			}
			else {
				GLCALL(glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount));

			}
			glBindTexture(GL_TEXTURE_2D, NULL);


			GLCALL(glBindVertexArray(NULL));
		}
	}

	GLCALL(glUseProgram(NULL));
}

