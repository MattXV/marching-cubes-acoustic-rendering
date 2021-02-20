#pragma once

#include <glfw/glfw3.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#if _DEBUG == 1
	#define GLCALL(x) unda::utils::ClearGLErrors(); x; if(!unda::utils::LogGlError()) { __debugbreak();};
#else
	#define GLCALL(x) x;
#endif

namespace unda {
	namespace utils {

		inline void ClearGLErrors() { while (glGetError() != GL_NO_ERROR); }
		inline bool LogGlError() {
			while (GLenum error = glGetError() != GL_NO_ERROR) {
				std::cout << "[GL ERROR]: " << error << std::endl;
				return false;
			}
			return true;
		}

		std::string ReadTextFile(const std::string& shaderPath);
		std::string StemFileName(const std::string& fileName);
	}
}


