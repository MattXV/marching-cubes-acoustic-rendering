#pragma once
#include "Engine.h"
#include "Time.h"


namespace unda {
	class UndaAPI {
	public:
		UndaAPI();
		~UndaAPI();

		void run();
		UndaAPI* getInstance() { return singleton; }
		void* getWindow() { return engine->getWindow(); }

	private:
		double lastFrameTime = 0.0;
		void* window;
		static UndaAPI* singleton;
		Engine* engine;
	};

}

