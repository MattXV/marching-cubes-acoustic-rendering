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

	Scene::Scene()
		: camera(new unda::FPSCamera(90.0f, (float)unda::windowWidth / (float)windowHeight, 0.001f, 900.0f))
		, boundingBoxRenderer(*camera)
	{
		json configuration = json::parse(utils::ReadTextFile("conf.json"));

		auto [vertices, indices] = unda::primitives::createSphere(16, 0.2f);
		Light* light = new Light(vertices, indices);
		light->setPosition(glm::vec3(2.0f, 9.5f, 7.0f));
		addLight(light);
		camera->setPosition(glm::vec3(1.0f, 0.0f, 1.0f));
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
		MarchingCubes* marchingCubes = new MarchingCubes(cellsPerDimension, 1, (float)inputScene->getModelScale() / cellsPerDimension, Point3D(0, 0, 0));
		marchingCubes->setGeneratePatches(generatePatches);

		utils::Timer timer{ "Computing Scalar Field + patches" };

		timer.start();
		marchingCubes->computeScalarField(inputScene);
		timer.stop();
		
		// TODO: FIX PATCH GENERATION IN MARCHING CUBES IMPLEMENTATION
		// UV CROPPING IS WRONG
		// DEBUG VISUALLY
		// INFER FROM IMAGE PATCHES.
		if (generatePatches) {
			for (std::pair<AABB, std::vector<TexturePatch>>& aabbCube : MarchingCubesPatches) {
				BoundingBox* aabb = new BoundingBox(aabbCube.first);
				for (TexturePatch patch : aabbCube.second) {
					if (patch.pixels.size() == 0) continue;
					aabb->doPatch(patch, static_cast<CubeMap::Face>(patch.cubeMapFace));
				}
				boundingBoxRenderer.getBoundingBoxes().push_back(std::unique_ptr<BoundingBox>(aabb));
			}
		}
			 
		timer.reset("Computing Marching Cubes");
		timer.start();
		marchingCubes->computeMarchingCubes(1.0);
		timer.stop();

		marchingCubesModel.reset(marchingCubes->createModel());
		for (auto& mesh : marchingCubesModel->getMeshes()) {
			mesh.transform = glm::translate(glm::mat4(1.0f), glm::vec3(-10, 0, -10));
			//mesh.transform = glm::scale(mesh.transform, glm::vec3(10, 10, 10));
		}
		glm::vec3 boundingVolume = marchingCubesModel->calculateBoundingVolume();
		
		UNDA_LOG_MESSAGE("width: " + std::to_string(inputScene->getVolume().x) + "height: " + std::to_string(inputScene->getVolume().y) + "depth: " + std::to_string(inputScene->getVolume().z));




		std::vector<acoustics::SurfacePatch> patches;
		//if (acoustics::loadPredictions("classifier/results.csv", patches) < 0) { UNDA_ERROR("Could not load patches"); return; }
		////std::array<std::array<double, 6>, 6> betaCoefficients = {
		////	acoustics::Materials::wallTreatments.getBetaCoefficients(),
		////	acoustics::Materials::wallTreatments.getBetaCoefficients(),
		////	acoustics::Materials::wallTreatments.getBetaCoefficients(),
		////	acoustics::Materials::wallTreatments.getBetaCoefficients(),
		////	acoustics::Materials::wallTreatments.getBetaCoefficients(),
		////	acoustics::Materials::wallTreatments.getBetaCoefficients()
		////};

		std::array<std::array<double, 6>, 6> betaCoefficients{};
		betaCoefficients[2] = acoustics::Materials::ceramic.getBetaCoefficients();
		betaCoefficients[3] = acoustics::Materials::ceramic.getBetaCoefficients();
		betaCoefficients[0] = acoustics::Materials::ceramic.getBetaCoefficients();
		betaCoefficients[1] = acoustics::Materials::ceramic.getBetaCoefficients();
		betaCoefficients[4] = acoustics::Materials::ceramic.getBetaCoefficients();
		betaCoefficients[5] = acoustics::Materials::ceramic.getBetaCoefficients();

			//betaCoefficients.fill(acoustics::Materials::defaultMaterial.getBetaCoefficients());
		//betaCoefficients.fill({ 0.0 , 0.0 , 0.0 , 0.0, 0.0, 0.0 });


		glm::vec3 sceneVolume = inputScene->getVolume();
		std::array<double, 3> spaceDimensions = { (double)sceneVolume.x, (double)sceneVolume.y, (double)sceneVolume.z };
		std::array<double, 3> source = configuration["IR"]["SourcePosition"].get<std::array<double, 3>>();
		std::array<double, 3> listener = configuration["IR"]["ListenerPosition"].get<std::array<double, 3>>();

		int nThreads = 30, ISM_sampleRate = (int)unda::sampleRate, nTaps = 2048; //11025
		int nSamples = (int)std::round((double)ISM_sampleRate * configuration["IR"]["TailLength"].get<double>());
		acoustics::ImageSourceModel* ism = new acoustics::ImageSourceModel(nThreads, spaceDimensions, source, listener, betaCoefficients);
		ism->setSamplingFrequency(ISM_sampleRate);
		ism->setNSamples(nSamples);
		//ism->doPatches(patches, cellsPerDimension);
		if (configuration["IR"]["GenerateIR"].get<int>())
			ism->generateIR();

		delete ism;	

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
		modelRenderer.render();
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