#pragma once

#include "../input/IEventHandler.h"
#include "../utils/Settings.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <assert.h>
#include <functional>


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
	Input input;
	GLFWwindow* window;


};

inline void errorCallback(int error, const char* description) {
	std::cerr << "[GLFW Error]: " << error << std::endl;
}

inline void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	else
		switch (action) {
			case GLFW_PRESS:
				input->onKeyDown(key, false);
				break;
			case GLFW_RELEASE:
				input->onKeyUp(key, false);
				break;
			case GLFW_REPEAT:
				input->onKeyDown(key, true);
				break;
		}
}