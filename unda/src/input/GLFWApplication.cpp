#include "GLFWApplication.h"

void GLAPIENTRY
GLMessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

unda::Time* unda::Time::singleton = nullptr;
void* unda::Input::window = nullptr;
unda::Input* unda::Input::singletonInstance = nullptr;

unda::GLFWInput::~GLFWInput()
{
	callbacks.clear();
}

bool unda::GLFWInput::isKeyDownImplementation(int keycode)
{
	
	int state = glfwGetKey((GLFWwindow*)window, keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool unda::GLFWInput::isKeyUpImplementation(int keycode)
{
	int state = glfwGetKey((GLFWwindow*)window, keycode);
	return state == GLFW_RELEASE;
}

bool unda::GLFWInput::isMouseButtonDownImplementation(int button)
{
	int state = glfwGetMouseButton((GLFWwindow*)window, button);
	return state == GLFW_PRESS;
}

bool unda::GLFWInput::isMouseButtonUpImplementation(int button)
{
	int state = glfwGetMouseButton((GLFWwindow*)window, button);
	return state == GLFW_RELEASE;
}

std::pair<double, double> unda::GLFWInput::getMousePositionImplementation()
{
	double x, y;
	glfwGetCursorPos((GLFWwindow*)window, &x, &y);
	return std::pair<double, double>(x, y);
}

void unda::GLFWInput::lockCursorImplementation(bool locked)
{
	glfwSetInputMode((GLFWwindow*)window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void unda::GLFWInput::keyCallBackImplementation(int keyCode)
{
	//std::lock_guard<std::mutex> guard(callbacksMutex);
	if (callbacks.find(keyCode) != callbacks.end()) {
		callbacks[keyCode]();
	}
}

void unda::GLFWInput::registerKeyCallBackImplementation(int keyCode, std::function<void()> newCallBack)
{
	std::lock_guard<std::mutex> guard(callbacksMutex);
	if (auto found = callbacks.find(keyCode) != callbacks.end()) {
		std::cerr << "Callback already existing brah..." << std::endl;
		return;
	} else {
		callbacks.emplace(std::make_pair(keyCode, newCallBack));
	}
}

// ----------------------------------------------------------------------------

unda::GLFWApplication::GLFWApplication()
	: window(nullptr)
	, time(new GLFWTime())
	, input(new GLFWInput())
{
	// Initialise window and create OpenGL Context
	if (!glfwInit()) {
		const char* message;
		glfwGetError(&message);
		std::cerr << "[GLFW Error] Could not initialise! " << message << std::endl;
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	window = glfwCreateWindow(unda::windowWidth, unda::windowHeight, "unda", NULL, NULL);
	assert(window);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(unda::vSync);
	glfwSetKeyCallback(window, keyCallBack);

	Input::setInstance(input);
	Input::setWindow(window);
	Time::setInstance(time);

	glfwSetErrorCallback(errorCallback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "[GLAD Error]: Could not initialise GL!" << std::endl;
	}
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	running = true;

	std::string versionString = std::string(glfwGetVersionString());
	std::string glVersion = "Loaded OpenGL, Version: ";
	glVersion.append(std::to_string(GLVersion.major) + "." + std::to_string(GLVersion.minor));
	UNDA_LOG_MESSAGE("Loaded GLFW, Version: " + versionString);
	UNDA_LOG_MESSAGE(glVersion);



	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(GLMessageCallback, 0);
}

unda::GLFWApplication::~GLFWApplication() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	delete time;
	delete input;
	glfwDestroyWindow(window);
	glfwTerminate();
}

void unda::GLFWApplication::processEvents()
{
	if (glfwWindowShouldClose(window))
		running = false;

	glfwSwapBuffers(window);
	glfwPollEvents();
}

