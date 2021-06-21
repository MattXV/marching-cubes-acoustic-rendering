#include "core/UndaAPI.h"
#include <iostream>


int main(int argc, char* argv[]) {
	unda::UndaAPI* apiApplication = new unda::UndaAPI();
	apiApplication->run();

	delete apiApplication;
	UNDA_LOG_MESSAGE("Exit");
	return 0;
}