#include "Scene.h"
#include <chrono>
#include <random>


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

		GLCALL(glBindVertexArray(NULL));
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, NULL));
		if (hasIndices) {
			GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL));

		}

	}

	Light::~Light()
	{
		if (hasIndices)
			glDeleteBuffers(1, &ibo);
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}


	// ---------------------------------------------------------------------------

	Scene::Scene() :
		boundingBoxRenderer(nullptr)
	{
		init();
	}
	void Scene::init() {
		camera = new FPSCamera(90.0f, (float)unda::windowWidth / (float)windowHeight, 0.001f, 900.0f);


		boundingBoxRenderer.setCamera(camera);

		json configuration = json::parse(utils::ReadTextFile("conf.json"));

		auto [vertices, indices] = unda::primitives::createSphere(16, 0.2f);
		Light* light = new Light(vertices, indices);
		light->setPosition(glm::vec3(2.0f, -19.5f, 7.0f));
		addLight(light);
		camera->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		modelRenderer.setCamera(camera);
		modelRenderer.setLightPosition(light->getPosition());
		modelRenderer.setLightColour(light->getColour());
		// Input Scene
		inputScene.reset(loadSceneGraph(configuration["Scene"]["SceneGraphFile"].get<std::string>()));
		//inputScene->normaliseMeshes();
		inputScene->calculateAABB();
		for (Mesh& mesh : inputScene->getMeshes())
			boundingBoxRenderer.getBoundingBoxes().push_back(std::make_unique<BoundingBox>(mesh.aabb));


		int cellsPerDimension = configuration["GeometryReduction"]["MarchingCubesResolution"].get<int>();
		bool generatePatches = (bool)configuration["GeometryReduction"]["GeneratePatches"].get<int>();
		glm::vec3 sceneVolume = inputScene->getVolume();

		MarchingCubes* marchingCubes = new MarchingCubes(cellsPerDimension, 1, ((1.0f / (float)cellsPerDimension) * (float)glm::length(sceneVolume)) * 0.8f, Point3D(0, 0, 0));
		marchingCubes->setGeneratePatches(generatePatches);


		marchingCubes->computeScalarField(inputScene);
		
		// TODO: FIX PATCH GENERATION IN MARCHING CUBES IMPLEMENTATION
		// UV CROPPING IS WRONG
		// DEBUG VISUALLY
		// INFER FROM IMAGE PATCHES.
		//if (generatePatches) {
		//	for (std::pair<AABB, std::vector<TexturePatch>>& aabbCube : MarchingCubesPatches) {
		//		BoundingBox* aabb = new BoundingBox(aabbCube.first);
		//		for (TexturePatch patch : aabbCube.second) {
		//			if (patch.pixels.size() == 0) continue;
		//			aabb->doPatch(patch, CubeMap::Face::NEGATIVE_X);
		//			aabb->doPatch(patch, CubeMap::Face::NEGATIVE_Y);
		//			aabb->doPatch(patch, CubeMap::Face::NEGATIVE_Z);
		//			aabb->doPatch(patch, CubeMap::Face::POSITIVE_X);
		//			aabb->doPatch(patch, CubeMap::Face::POSITIVE_Y);
		//			aabb->doPatch(patch, CubeMap::Face::POSITIVE_Z);
		//		}
		//		boundingBoxRenderer.getBoundingBoxes().push_back(std::unique_ptr<BoundingBox>(aabb));
		//	}
		//}


		marchingCubes->computeMarchingCubes(0.0);

		marchingCubesModel.reset(marchingCubes->createModel());
		for (auto& mesh : marchingCubesModel->getMeshes()) {
			//mesh.transform = glm::translate(mesh.transform, glm::vec3(-glm::length(sceneVolume)));
			//mesh.transform = glm::scale(mesh.transform, glm::vec3(10, 10, 10));
		}
		glm::vec3 boundingVolume = marchingCubesModel->calculateBoundingVolume();
		
		configuration["Scene"]["Dimensions"] = { inputScene->getVolume().x, inputScene->getVolume().y, inputScene->getVolume().z };
		{
			std::ofstream o("conf.json");
			o << std::setw(4) << configuration << std::endl;
		}
		if (!configuration["IR"]["GenerateIR"].get<int>()) return;

		std::array<std::array<double, 6>, 6> betaCoefficients, alphaCoeffiecients;
		alphaCoeffiecients = configuration["IR"]["SurfaceAbsorption"].get<std::array<std::array<double, 6>, 6>>();
		for (int row = 0; row < 6; row++) {
			for (int bin = 0; bin < 6; bin++) {
				betaCoefficients[row][bin] = sqrt(1 - alphaCoeffiecients[row][bin]);
			}
		}

		//glm::vec3 sceneVolume = inputScene->getVolume();
		std::array<double, 3> spaceDimensions = { (double)sceneVolume.x, (double)sceneVolume.y, (double)sceneVolume.z };
		std::array<double, 3> source = configuration["IR"]["SourcePosition"].get<std::array<double, 3>>();
		std::array<double, 3> listener = configuration["IR"]["ListenerPosition"].get<std::array<double, 3>>();

		int nThreads = 30, ISM_sampleRate = (int)unda::sampleRate, nTaps = 2048; //11025
		int nSamples = (int)std::round((double)ISM_sampleRate * configuration["IR"]["TailLength"].get<double>());
		acoustics::ImageSourceModel* ism = new acoustics::ImageSourceModel(spaceDimensions, source, listener, betaCoefficients, nSamples, configuration["IR"]["Order"].get<unsigned int>());
		if (configuration["IR"]["GenerateIR"].get<int>())
			ism->dispatchCPUThreads();

		delete ism;	
		{
			//Filter filter = Filter(Filter::filterType::BPF, 300, 3000);
			Signal ir = ReadAudioFileIntoMono("ir.wav");
			Signal audio = ReadAudioFileIntoMono("drums.wav");
			Signal out = FFTConvolution(audio, ir);
			NormaliseSignal(out);
			WriteAudioFile({ out }, "test_reverb.wav");
		}
	
	}


	Scene::~Scene()
	{
		boundingBoxRenderer.cleanUp();
		models.clear();

		std::for_each(lights.begin(), lights.end(), [](Light* light) { delete light; });
		for (auto& texture : loadedTextures)
			delete texture.second.release();
		//for (std::pair<const std::string, std::vector<std::unique_ptr<Texture>>>& textureEntry : loadedTextures) {
		//	for (auto& texture : textureEntry.second) delete texture.release();
		//	// Manual deletion is to dispose GL objects before context gets deleted.
		//}
		delete camera;
	}

	void Scene::render() {
		modelRenderer.setModel(inputScene.get());
		modelRenderer.render();
		modelRenderer.setModel(marchingCubesModel.get());
		modelRenderer.render(true);
		boundingBoxRenderer.render();
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