#pragma once
#include "Scene.h"

namespace unda {
	class TestScene : public Scene {
	public:
		TestScene();
		~TestScene();

		void update();
		void render();
	private:
		virtual void init() override;

		ModelRenderer renderer;
		FrameBuffer frameBuffer;
		GUIImage gui;
		//FPSCamera camera = FPSCamera(90.0f, (float)windowWidth / (float)windowHeight, 0.001, 900.0f);
		Model* model;
		Camera* perspective = new FPSCamera(90.0f, (float)windowWidth / (float)windowWidth, 0.001, 300.0f);
	};

}