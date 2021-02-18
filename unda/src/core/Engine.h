#pragma once

#include "../unda.h"

#include <math.h>

#include <iostream>
#include <assert.h>

class GLFWApplication;

class Engine
{
public:
	Engine();
	Engine(int width, int height);
	~Engine();

	bool keepRunning();
	void update();
	void render();

private:
	GLFWApplication* application;

	// Program
	bool init();

};