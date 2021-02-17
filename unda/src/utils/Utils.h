#pragma once

#include <glfw/glfw3.h>
#include <iostream>

#if _DEBUG == 1
	#define GLCALL(x) utils::glClearErrors(); x; if(!utils::logGlError()) { __debugbreak();};
#else
	#define GLCALL(x) x;
#endif

namespace utils {
	inline void glClearErrors() { while (glGetError() != GL_NO_ERROR); }
	inline bool logGlError() {
		while (GLenum error = glGetError() != GL_NO_ERROR) {
			std::cout << "[GL ERROR]: " << error << std::endl;
			return false;
		}
		return true;
	}
}


