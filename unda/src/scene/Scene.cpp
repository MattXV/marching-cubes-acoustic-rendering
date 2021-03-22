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
				indexCount = (unsigned int)indices.size();
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
		light->setPosition(glm::vec3(2.0f, 2.5f, 7.0f));
		addLight(light);

		camera->setPosition(glm::vec3(1.0f, 0.0f, 1.0f));


		// Marching cubes
		
		// Parse DMT Room

		//std::unique_ptr<Model> model(unda::loadModel("resources/models/ChibiCarlo/ChibiCarlo.obj", Colour<float>(0.973445f, 0.791298f, 0.62396f, 1.0f)));
		Model* model = unda::loadModel("resources/models/ChibiCarlo/ChibiCarlo.obj", Colour<float>(0.973445f, 0.791298f, 0.62396f, 1.0f ));
		model->normaliseMeshes();
		model->calculateAABB();
		model->setPosition(glm::vec3(2.0f, 0.0f, 0.0f));
		



		Model* conferenceModel = loadMeshDirectory("resources/models/ConferenceRoom/Models", "fbx", Colour<float>(0.2f, 0.2f, 0.2f, 1.0f), true);
		conferenceModel->normaliseMeshes();
		//conferenceModel->computeEnvelopes();
		conferenceModel->calculateAABB();

		// Vector-based marching cubes
		const size_t resolution = 200;
		unda::LatticeVector3D<float> latticeData{resolution, resolution, resolution };
		computeScalarFieldFromMeshes(latticeData, conferenceModel);

		float gridSpacing = 0.01f;
		unda::ScalarFieldVector3D grid{ gridSpacing, unda::Point3D(0.0f, 0.0f, 0.0f), latticeData };
		double isoLevel = 1.0f;

		std::vector<Vertex> vertexData = grid.computeVertexData(isoLevel);

		Texture* mTexture = new Texture(1024, 1024, Colour<unsigned char>(255, 255, 255, 255));
		Model* marchingCubes = new Model((std::vector<Vertex>&&)vertexData, std::vector<unsigned int>(), mTexture);
		//marchingCubes->normaliseMeshes();
		//marchingCubes->normaliseMeshes();
		marchingCubes->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
		marchingCubes->setPosition(glm::vec3(2.0f, 0.0f, 2.0f));
		

		conferenceModel->toVertexArray();
		marchingCubes->toVertexArray();
		model->toVertexArray();
		
		addModel(model);
		addModel(marchingCubes);
		addModel(conferenceModel);

		
		std::vector<std::vector<double>> rr = { { 0, 0, 0 }, {0, 1, 0} };
		std::vector<double> ss{1.0};

		std::vector<std::vector<double>> rir = gen_rir(343, 44100, rr, ss, { 1.0, 1.0, 1.0 }, { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 }, { 0, 0 }, 0, 3);
	}

	Scene::~Scene()
	{
		models.clear();
		std::for_each(lights.begin(), lights.end(), [](Light* light) { delete light; });
		delete camera;
	}

	void Scene::update()
	{
		camera->handleInput();
	}

	void Scene::addModel(unda::Model* newModel)
	{
		models.push_back(std::unique_ptr<Model>(newModel));
	}
	void Scene::addLight(Light* newLight) {
		lights.push_back(newLight);
	}
}