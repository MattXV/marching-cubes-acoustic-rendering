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
		light->setPosition(glm::vec3(10.0f, 2.5f, 10.0f));
		addLight(light);

		

		// Marching cubes
		
		// Parse DMT Room
		//std::string porcoddio = utils::ReadTextFile("resources/models/dmt_conference.txt");
		//unda::utils::PlyParser parser = unda::utils::PlyParser("C:/Users/matt/git/unda/unda/resources/models/test.txt");

		// Get data from the object
		/*
		happly::PLYData plyIn("resources/models/dmt_conference.txt", true);
		std::vector<float> verticesX = plyIn.getElement("vertex").getProperty<float>("x");
		std::vector<float> verticesY = plyIn.getElement("vertex").getProperty<float>("y");
		std::vector<float> verticesZ = plyIn.getElement("vertex").getProperty<float>("z");
		undaTesting::LatticeVector3D<float> scalarField{};
		scalarField.assignFloatVectors(verticesX, verticesY, verticesZ);
		undaTesting::ScalarFieldVector3D* grid = new undaTesting::ScalarFieldVector3D( gridSpacing, undaTesting::Point3D(0.0f, 0.0f, 0.0f), scalarField );
		*/
		
		
		const size_t nx = 30, ny = 30, nz = 30;
		
		//LatticeData3D<float, nx, ny, nz> scalarField{unda::heightMapTerrain("resources/models/terrain-heightmap.png")};
		Point centre(0.0f, 0.0f, 0.0f);
		//assignScalarField(scalarField, centre);


		float gridSpacing = 1.0f;
		//CubeLatticeScalarField3D<nx, ny, nz> grid{ gridSpacing, centre, scalarField };
		double isoLevel = 1.0f;

		std::vector<Vertex> vertexData;
		//vertexData = grid.computeVertexData(isoLevel);

		
		//Texture* mTexture = new Texture("resources/models/terrain-texture.png");
		//Model* marchingCubes = new Model(vertexData, std::vector<unsigned int>(), mTexture);
		//marchingCubes->setRotation(glm::vec3(0, 0, 0));
		//addModel(marchingCubes);
		Model* model = unda::loadModel("resources/models/dmt_conference.obj", "resources/models/dmt_conference_texture.png");
		//model->setPosition(glm::vec3(10.0f, 0.0f, 10.0f));
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