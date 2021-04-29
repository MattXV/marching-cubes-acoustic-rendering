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
		auto modelpath = std::filesystem::path("resources/models/ConferenceRoom/Models");
		modelpath = modelpath.make_preferred();
		std::shared_ptr<Model> conferenceModel = std::shared_ptr<Model>(loadMeshDirectory(modelpath.string(), "fbx", Colour<float>(0.2f, 0.2f, 0.2f, 1.0f), true));
		if (conferenceModel->getMeshes().empty()) { std::cout << "No meshes!" << std::endl; return; }
		conferenceModel->normaliseMeshes();
		conferenceModel->calculateAABB();
		conferenceModel->setScale(glm::vec3(10, 10, 10));
		//conferenceModel->computeEnvelopes();

		int unique = 0;
		for (Mesh& mesh : conferenceModel->getMeshes()) {
			Model* model = unda::primitives::cubeBoundingBox(mesh.aabb);
			boundingBoxesModels.emplace_back(std::unique_ptr<Model>(model));
			boundingBoxes.insert(std::make_pair(mesh.name + std::to_string(unique++), model));
			model->setScale(glm::vec3(3.0f, 3.0f, 3.0f));
			model->toVertexArray();
		}

		MarchingCubes* marchingCubes = new MarchingCubes(45, 32, 1.0f, Point3D(0, 0, 0));
		marchingCubes->computeScalarField(conferenceModel);
		marchingCubes->computeMarchingCubes(1.0);
		Model* marchingCubesModel = marchingCubes->createModel();
		marchingCubesModel->normaliseMeshes();
		marchingCubesModel->setPosition(glm::vec3(2.0f, 0.0f, 2.0f));


		marchingCubesModel->toVertexArray();
		conferenceModel->toVertexArray();
		//addModel(conferenceModel);
		addModel(marchingCubesModel);
		
		std::array<std::array<double, 6>, 6> betaCoefficients = {
			acoustics::Materials::carpet.getBetaCoefficients(),
			acoustics::Materials::floor.getBetaCoefficients(),
			acoustics::Materials::wallTreatments.getBetaCoefficients(),
			acoustics::Materials::wallTreatments.getBetaCoefficients(),
			acoustics::Materials::wallTreatments.getBetaCoefficients(),
			acoustics::Materials::wallTreatments.getBetaCoefficients(),
		};
		

		int nThreads = 32, ISM_sampleRate = (int)unda::sampleRate, nTaps = 2048; //11025
		int nSamples = (int)std::round((double)ISM_sampleRate * 0.2);
		std::array<double, 3> spaceDimensions{ 3.5, 3.5, 2.8 };
		std::array<double, 3> listener{ 1.05, 1.2, 1.68 };
		std::array<double, 3> source{ 3.0, 1.2, 0.6 };
		
		acoustics::ImageSourceModel* ism = new acoustics::ImageSourceModel(nThreads, spaceDimensions, source, listener, betaCoefficients);
		ism->setSamplingFrequency(ISM_sampleRate);
		ism->setNSamples(nSamples);
		//ism->generateIR();

		delete ism;	
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
		models.push_back(std::shared_ptr<Model>(newModel));
	}
	void Scene::addModel(std::shared_ptr<Model>& newModel)
	{
		models.push_back(newModel);
	}

	void Scene::addLight(Light* newLight) {
		lights.push_back(newLight);
	}
}