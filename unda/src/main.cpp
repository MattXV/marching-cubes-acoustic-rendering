#include "core/Engine.h"


int main(int argc, char* argv[]) {
	Engine* engine = new Engine(800, 600);

	engine->render();
	while (engine->keepRunning()) {
		engine->update();
		engine->render();
	}
	
	delete engine;
	return 0;
}