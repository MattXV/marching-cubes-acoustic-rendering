#include "GLFWApplication.h"


void Input::onKeyDown(uint32_t keyCode, bool isRepeat)
{
	if (isRepeat) {
		std::cout << "key repeat! " << std::endl;
		return;
	}
	std::cout << "pressed a key! " << std::endl;

}

void Input::onKeyUp(uint32_t keyCode, bool isRepeat)
{

}


GLFWApplication::GLFWApplication()
	: input()
	, window(nullptr)
{
	// Initialise window and create OpenGL Context
	if (!glfwInit()) {
		const char* message;
		glfwGetError(&message);
		std::cerr << "[GLFW Error] Could not initialise! " << message << std::endl;
		return;
	}
	window = glfwCreateWindow(Settings::windowWidth, Settings::windowHeight, "unda", NULL, NULL);
	assert(window);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwMakeContextCurrent(window);

	glfwSwapInterval(Settings::vSync);

	glfwSetWindowUserPointer(window, &input);
	glfwSetKeyCallback(window, keyCallBack);

	glfwSetErrorCallback(errorCallback);
	gladLoadGL();

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	running = true;

}

GLFWApplication::~GLFWApplication() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void GLFWApplication::processEvents()
{
	if (glfwWindowShouldClose(window))
		running = false;

	glfwSwapBuffers(window);
	glfwPollEvents();
}

