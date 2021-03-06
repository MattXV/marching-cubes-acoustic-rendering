#include "ModelRenderer.h"

ModelRenderer::ModelRenderer() : shaders("resources/shaders/vertex_shader.glsl", "resources/shaders/fragment_shader.glsl")
{

}

void ModelRenderer::render()
{
	if (!model) return;
	if (!camera) return;
	shaders.attach();
	GLCALL(glDisable(GL_CULL_FACE));
	GLCALL(glUniformMatrix4fv(shaders.getUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix())));
	GLCALL(glUniformMatrix4fv(shaders.getUniformLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix())));

	// Fragment shader uniforms
	GLCALL(glUniform3fv(shaders.getUniformLocation("lightColour"), 1, glm::value_ptr(lightColour)));
	GLCALL(glUniform3fv(shaders.getUniformLocation("lightPosition"), 1, glm::value_ptr(lightPosition)));
	GLCALL(glUniform3fv(shaders.getUniformLocation("viewPosition"), 1, glm::value_ptr(camera->getPosition())));
	for (unda::Mesh& mesh : model->getMeshes()) {
		//if (mesh.meshFileName.find("terrain") != std::string::npos) __debugbreak();
		GLCALL(glBindVertexArray(mesh.vao));
			GLCALL(glUniformMatrix4fv(shaders.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(mesh.transform)));
			GLCALL(glActiveTexture(GL_TEXTURE0));
			GLCALL(glUniform1i(shaders.getUniformLocation("textureSampler"), 0));
			GLCALL(glBindTexture(GL_TEXTURE_2D, mesh.texture->getTextureId()));

			if (mesh.indices->size() > 0) {
				//GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
				GLCALL(glDrawElements(GL_TRIANGLES, mesh.indices->size(), GL_UNSIGNED_INT, nullptr));
				//GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

			}
			else {
				//GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
				GLCALL(glDrawArrays(GL_TRIANGLES, 0, mesh.vertices->size()));
				//GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
			}
	}
			GLCALL(glBindTexture(GL_TEXTURE_2D, NULL));
		GLCALL(glBindVertexArray(NULL));
	GLCALL(glEnable(GL_CULL_FACE));
	shaders.detach();
}


