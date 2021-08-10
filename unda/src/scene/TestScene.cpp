#include "TestScene.h" 

namespace unda {
	TestScene::TestScene() :
		gui(640, 640),
		renderer(),
		frameBuffer(64, 64)
	{
		gui.setTexture(frameBuffer.getTextureLocation());
		init();

		std::function<void()> saveImage = [&]() {
			unsigned char* image = frameBuffer.getImage();
			stbi_flip_vertically_on_write(1);
			int written = stbi_write_png("output/test.png", frameBuffer.getWidth(), frameBuffer.getHeight(), STBI_rgb, (void*)image, frameBuffer.getWidth() * STBI_rgb * sizeof(unsigned char));
			delete[] image;
			if (!written) UNDA_ERROR("Image write failure!");
		};
		Input::registerKeyCallBack(Key::E, saveImage);
	}

	TestScene::~TestScene() {
		delete model;
		delete perspective;
	}

	void TestScene::update() {
		//camera->update();
		//camera->handleInput();
		perspective->handleInput();
	}

	void TestScene::render() {
		frameBuffer.bind();
			renderer.setCamera(perspective);
			unda::render::prepare(glm::vec4(0.2f));
			renderer.render();
		frameBuffer.unbind();

		renderer.setCamera(perspective);
		renderer.render();
		gui.render();
	}
	void TestScene::init()
	{
		camera = new OrthoCamera((float)windowWidth, (float)windowHeight, 0.01, 200.0f);
		model = unda::primitives::createSphereModel(64, 1.2f);
		


		model->setPosition(glm::vec3(0.0f, 0.0f, -30.0f));
		renderer.setCamera(camera);
		renderer.setLightPosition(glm::vec3(0.0f, 2.0f, 1.0f));
		renderer.setLightColour(glm::vec3(0.7f, 0.3f, 0.1f));
		renderer.setModel(model);
	}
}