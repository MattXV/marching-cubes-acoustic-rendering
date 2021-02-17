#include "Engine.h"

static void errorCallback(int error, const char* description) {
	std::cerr << "[GLFW Error]: " << error << std::endl;
}

static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

Engine::Engine() 
	: window(nullptr)

{
	if (!init()) return;
}

Engine::Engine(int width, int height)
	: window(nullptr)
{
	Settings::windowWidth = width;
	Settings::windowHeight = height;

	if (!init()) return;
}

Engine::~Engine() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

bool Engine::keepRunning()
{
	while (!glfwWindowShouldClose(window))
		return true;
	return false;
}

void Engine::update()
{
}

void Engine::render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	////glEnable(GL_CULL_FACE);


	glfwSwapBuffers(window);
	glfwPollEvents();
}

bool Engine::init()
{
	// Initialise window and create OpenGL Context
	if (!glfwInit()) {
		const char* message;
		glfwGetError(&message);
		std::cerr << "[GLFW Error] Could not initialise! " << message << std::endl;
		return false;
	}
	window = glfwCreateWindow(Settings::windowWidth, Settings::windowHeight, "unda", NULL, NULL);
	assert(window);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(Settings::vSync);
	glfwSetErrorCallback(errorCallback);
	gladLoadGL();
	glfwSetKeyCallback(window, keyCallBack);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	return true;
}
