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
		light->setPosition(glm::vec3(5.0f, 2.5f, 5.0f));
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
		conferenceModel->calculateAABB();

		const size_t nx = 35, ny = 35, nz = 35;
		
		//LatticeData3D<float, nx, ny, nz> scalarField{unda::heightMapTerrain("resources/models/terrain-heightmap.png")};
		LatticeData3D<float, nx, ny, nz> scalarField{};
		Point centre(0.0f, 0.0f, 0.0f);
		computeScalarFieldFromMeshes(scalarField, conferenceModel);
		//assignScalarField(scalarField, centre);

		float gridSpacing = 1.0f;
		CubeLatticeScalarField3D<nx, ny, nz> grid{ gridSpacing, centre, scalarField };
		double isoLevel = 1.0f;

		std::vector<Vertex> vertexData;
		vertexData = grid.computeVertexData(isoLevel);

		
		Texture* mTexture = new Texture(1024, 1024, Colour<unsigned char>(255, 255, 255, 255));
		Model* marchingCubes = new Model((std::vector<Vertex>&&)vertexData, std::vector<unsigned int>(), mTexture);
		//marchingCubes->normaliseMeshes();
		marchingCubes->normaliseMeshes();
		marchingCubes->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
		marchingCubes->setPosition(glm::vec3(2.0f, 0.0f, 2.0f));

		model->toVertexArray();
		conferenceModel->toVertexArray();
		marchingCubes->toVertexArray();

		addModel(model);
		addModel(marchingCubes);
		addModel(conferenceModel);
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