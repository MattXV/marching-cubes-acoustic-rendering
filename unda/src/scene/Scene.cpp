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
		: acousticSpace(new unda::acoustics::AcousticSpace())
	{
		camera = new unda::FPSCamera(90.0f, (float)unda::windowWidth / (float)windowHeight, 0.1f, 90.0f);

		auto [vertices, indices] = unda::primitives::createSphere(16, 0.2f);
		Light* light = new Light(vertices, indices);
		light->setPosition(glm::vec3(2.0f, 2.5f, 7.0f));
		addLight(light);

		camera->setPosition(glm::vec3(1.0f, 0.0f, 1.0f));


		std::shared_ptr<Model> conferenceModel = std::shared_ptr<Model>(loadMeshDirectory("resources/models/ConferenceRoom/Models", "fbx", Colour<float>(0.2f, 0.2f, 0.2f, 1.0f), true));
		conferenceModel->normaliseMeshes();
		conferenceModel->calculateAABB();
		//conferenceModel->computeEnvelopes();

		int unique = 0;
		for (Mesh& mesh : conferenceModel->getMeshes()) {
			Model* model = unda::primitives::cubeBoundingBox(mesh.aabb);		
			boundingBoxesModels.emplace_back(std::unique_ptr<Model>(model));
			boundingBoxes.insert(std::make_pair(mesh.name + std::to_string(unique++), model));
			model->setScale(glm::vec3(3.0f, 3.0f, 3.0f));
			model->toVertexArray();
		}

		
		MarchingCubes* marchingCubes = new MarchingCubes(35, 32, 1.0f, Point3D(0, 0, 0));
		marchingCubes->computeScalarField(conferenceModel);
		marchingCubes->computeMarchingCubes(1.0);
		Model* marchingCubesModel = marchingCubes->createModel();
		marchingCubesModel->normaliseMeshes();
		marchingCubesModel->setPosition(glm::vec3(2.0f, 0.0f, 2.0f));

		marchingCubesModel->toVertexArray();
		conferenceModel->toVertexArray();
		//addModel(conferenceModel);
		addModel(marchingCubesModel);


		int nSamples = (int)(unda::sampleRate * 0.5);
		acousticSpace->setSpaceDimensions({ 3.5, 3.5, 2.8 });
		std::vector<std::vector<double>> rir = acousticSpace->generateRIR({ { 0.2, 0.3, 0.2 } },
			{ 0.9, 0.3, 0.1 }, nSamples);

		SF_INFO wavInfo = SF_INFO();
		wavInfo.samplerate = (int)unda::sampleRate;
		wavInfo.channels = 1;
		wavInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
		SNDFILE* sndFile = sf_open("ir.wav", SFM_WRITE, &wavInfo);
		std::cout << sf_strerror(sndFile) << std::endl;
		std::vector<double>& out = rir[0];
		sf_count_t written = sf_writef_double(sndFile, &out[0], (sf_count_t)out.size());
		std::cout << sf_error(sndFile) << std::endl;
		sf_close(sndFile);

		
	}

	Scene::~Scene()
	{
		models.clear();
		std::for_each(lights.begin(), lights.end(), [](Light* light) { delete light; });
		delete acousticSpace;
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