#include "UndaAPI.h"

namespace unda {
	UndaAPI* UndaAPI::singleton = nullptr;

	UndaAPI::UndaAPI()
		: window(nullptr)
	{
		engine = new Engine();
		singleton = this;
	}

	void UndaAPI::run() {
		while (engine->keepRunning()) {
			double time = Time::getTime();
			double deltaTime = time - lastFrameTime;
			Time::setDeltaTime(deltaTime);
			lastFrameTime = time;
			engine->update(deltaTime);
			engine->render();
		}
	}

	UndaAPI::~UndaAPI()
	{
		delete engine;
	}
}
