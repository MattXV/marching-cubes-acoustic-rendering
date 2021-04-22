#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <assert.h>
#include <mutex>
#include <functional>
#include "../utils/Settings.h"
#include "Input.h"
#include "../core/Time.h"


namespace unda {

	class GLFWTime : public Time {
	public:
	protected:
		double getTimeImplementation() override { return glfwGetTime(); }
		const double& getDeltaTimeImplementation() override { return deltaTime; }
		void setDeltaTimeImplementation(const double& newDeltaTime) override { deltaTime = newDeltaTime; }
	private:
		double deltaTime = 0.0;
	};

	class GLFWInput : public unda::Input {
	public:
		GLFWInput() { }
		~GLFWInput();

	protected:
		bool isKeyDownImplementation(int keycode) override;
		bool isKeyUpImplementation(int keycode) override;

		bool isMouseButtonDownImplementation(int button) override;
		bool isMouseButtonUpImplementation(int button) override;
		std::pair<double, double> getMousePositionImplementation() override;
		void lockCursorImplementation(bool locked) override;

		void keyCallBackImplementation(int keyCode) override;
		void registerKeyCallBackImplementation(int keyCode, std::function<void()> newCallBack) override;

	private:
		std::mutex callbacksMutex;
		std::unordered_map<int, std::function<void()>> callbacks;
	};

	// -------------------------------------------------------------------------

	class GLFWApplication {
	public:
		GLFWApplication();
		virtual ~GLFWApplication();

		void processEvents();
		bool isRunning() { return running; }
		void* getWindow() { return (void*)window; }

	private:
		GLFWTime* time;
		GLFWInput* input;
		bool running = false;
		GLFWwindow* window;
	};
}

inline void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));

	unda::Input::keyCallBack(key);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	else
		switch (action) {
			case GLFW_PRESS:
				//input->onKeyDown(key, false);
				break;
			case GLFW_RELEASE:
				//input->onKeyUp(key, false);
				break;
			case GLFW_REPEAT:
				//input->onKeyDown(key, true);
				break;
		}
}

inline void errorCallback(int code, const char* description)
{
	std::cout << description << std::endl;
	std::cerr << "[GLFW Error]: " << code << std::endl;
}
