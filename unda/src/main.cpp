#include "core/Engine.h"


int main(int argc, char* argv[]) {
	Engine* engine = new Engine(800, 600);

	while (engine->keepRunning()) {
		engine->update();
		engine->render();
	}
	
	delete engine;
	return 0;
}