#pragma once

#include "../unda.h"

#include <iostream>
#include <assert.h>


class Input : public IEventHandler {
public:
	Input() {};
	~Input() = default;

	void onKeyDown(uint32_t keyCode, bool isRepeat);
	void onKeyUp(uint32_t keyCode, bool isRepeat);

private:

};


class GLFWApplication {
public:
	GLFWApplication();
	virtual ~GLFWApplication();

	void processEvents();
	bool isRunning() { return running; }

private:
	bool running = false;
	Input* input;
	GLFWwindow* window;

};


inline static void errorCallback(int error, const char* description) {
	std::cerr << "[GLFW Error]: " << error << std::endl;
}

inline static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}