#include "Scene.h"

namespace unda {

	Light::Light(const std::vector<Vertex>& vertices, const std::vector<unsigned int> indices) {
		GLCALL(glGenVertexArrays(1, &vao));
		GLCALL(glGenBuffers(1, &vbo));
		if (!indices.empty()) {
			hasIndices = true;
			GLCALL(glGenBuffers(1, &ibo));
		}
		GLCALL(glBindVertexArray(vao));

		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		GLCALL(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW));
		
		GLCALL(glVertexAttribPointer(unda::shaders::lightVertexPositionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr));
		GLCALL(glEnableVertexAttribArray(unda::shaders::lightVertexPositionLocation)); // vertexPosition
		
		GLCALL(glVertexAttribPointer(unda::shaders::lightVertexNormalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx)));
		GLCALL(glEnableVertexAttribArray(unda::shaders::lightVertexNormalLocation)); // vertexNormal
		if (hasIndices) {
			indexCount = indices.size();
			GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
			GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));
		}

		glBindVertexArray(NULL);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		if (hasIndices)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);

	}

	Light::~Light()
	{
		if (hasIndices)
			glDeleteBuffers(1, &ibo);
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}


	// ---------------------------------------------------------------------------

	Scene::Scene()
	{
		camera = new unda::FPSCamera(90.0f, (float)unda::windowWidth / (float)windowHeight, 0.1f, 90.0f);

		auto [vertices, indices] = unda::primitives::createSphere(16, 0.2f);
		Light* light = new Light(vertices, indices);
		light->setPosition(glm::vec3(1.0f, 2.5f, -1.5f));
		addLight(light);

		Model* model = unda::loadModel("resources/models/dmt_conference.obj", "resources/models/dmt_conference_texture.png");
		model->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		addModel(model);
	}

	Scene::~Scene()
	{
		std::for_each(models.begin(), models.end(), [](Model* model) { delete model; });
		std::for_each(lights.begin(), lights.end(), [](Light* light) { delete light; });
		delete camera;
	}

	void Scene::update()
	{
		camera->handleInput();
	}

	void Scene::addModel(unda::Model* newModel)
	{
		models.push_back(newModel);
	}
	void Scene::addLight(Light* newLight) {
		lights.push_back(newLight);
	}
}