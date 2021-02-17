#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <math.h>

#include <iostream>
#include <assert.h>
#include "../Settings.h"
#include "../utils/Utils.h"



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
	GLFWwindow* window;

	// Program
	bool init();

};