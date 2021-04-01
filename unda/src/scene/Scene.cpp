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
		: acousticSpace(new AcousticSpace())
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
		/*Model* model = unda::loadModel("resources/models/ChibiCarlo/ChibiCarlo.obj", Colour<float>(0.973445f, 0.791298f, 0.62396f, 1.0f ));
		model->normaliseMeshes();
		model->calculateAABB();
		model->setPosition(glm::vec3(2.0f, 0.0f, 0.0f));
		*/


		std::shared_ptr<Model> conferenceModel = std::shared_ptr<Model>(loadMeshDirectory("resources/models/ConferenceRoom/Models", "fbx", Colour<float>(0.2f, 0.2f, 0.2f, 1.0f), true));
		conferenceModel->normaliseMeshes();
		//conferenceModel->computeEnvelopes();
		conferenceModel->calculateAABB();

		
		MarchingCubes* marchingCubes = new MarchingCubes(150, 32, 1.0f, Point3D(0, 0, 0));
		marchingCubes->computeScalarField(conferenceModel);
		marchingCubes->computeMarchingCubes(1.0);

		Model* marchingCubesModel = marchingCubes->createModel();
		marchingCubesModel->normaliseMeshes();
		marchingCubesModel->setPosition(glm::vec3(2.0f, 0.0f, 2.0f));


		marchingCubesModel->toVertexArray();
		conferenceModel->toVertexArray();
		addModel(conferenceModel);
		addModel(marchingCubesModel);

		int nSamples = (int)(unda::sampleRate * 0.5);
		acousticSpace->setSpaceDimensions({ 3.5, 3.5, 2.8 });
		std::vector<std::vector<double>> rir = acousticSpace->generateRIR({ { 0.2, 0.3, 0.2 } },
			{ 0.9, 0.3, 0.1 }, nSamples);

		SF_INFO wavInfo = SF_INFO();
		wavInfo.samplerate = (int)unda::sampleRate;
		wavInfo.channels = 1;
		wavInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
		//wavInfo.sections = 0;
		//wavInfo.seekable = 1;
		SNDFILE* sndFile = sf_open("ir.wav", SFM_WRITE, &wavInfo);
		std::cout << sf_strerror(sndFile) << std::endl;
		std::vector<double>& out = rir[0];
		sf_count_t written = sf_writef_double(sndFile, &out[0], (sf_count_t)out.size());
		std::cout << sf_error(sndFile) << std::endl;
		sf_close(sndFile);
		/*
		std::shared_ptr<Model> sharedModel = std::shared_ptr<Model>(conferenceModel);
		MarchingCubes* multiThreadMC = new MarchingCubes(250, 32, 1.0f, Point3D(0, 0, 0));
		multiThreadMC->computeScalarField(sharedModel);
		
		unda::ScalarFieldVector3D grid{ 0.01f, unda::Point3D(0.0f, 0.0f, 0.0f), multiThreadMC->getScalarField() };
		std::vector<Vertex> vertexData = grid.computeVertexData(1.0);
		Texture* mTexture = new Texture(1024, 1024, Colour<unsigned char>(255, 255, 255, 255));
		Model* marchingCubes = new Model((std::vector<Vertex>&&)vertexData, std::vector<unsigned int>(), mTexture);


		marchingCubes->setScale(glm::vec3(1.5f, 1.5f, 1.5f));
		marchingCubes->setPosition(glm::vec3(2.0f, 0.0f, 2.0f));
		marchingCubes->normaliseMeshes();
		marchingCubes->toVertexArray();

		addModel(marchingCubes);


		std::vector<std::vector<double>> rr = { { 0, 0, 0 } };
		std::vector<double> ss = {1.0, 1.0, 1.0 };

		std::vector<std::vector<double>> rir = gen_rir(
			343, 48000,
			rr, ss, 
			{ 3.5, 3.5, 2.8 }, 
			{ 0.1, 0.1, 0.1, 0.2, 0.3, 0.3 },
			{ 0, 0 },  1, 3, -1, 0.5 * 48000);

*/
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