#pragma once

#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>
#include <glad/glad.h>
#include <vector>
#include <array>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>


#define DISABLE_COPY_ASSIGN(Class) Class(const Class&) = delete; void operator=(const Class&) = delete; 

#if UNDA_DEBUG == 1
	#define UNDA_ERROR(Message) std::cerr << Message << std::endl; __debugbreak();  
	#define GLCALL(call) unda::utils::clearGLError();\
		call;\
		if (!unda::utils::printGLError(#call, __FILE__, __LINE__))\
			__debugbreak()
#else
	#define UNDA_ERROR(Message) ;  
	#define GLCALL(call) call;
#endif


namespace unda {

	enum class errorSeverity {
		WARNING,
		CRITICAL,
		FATAL
	};
	namespace utils {
		inline void clearGLError() { while (GLenum error = glGetError()); }
		inline bool printGLError(const char* call, const char* file, int line) {
			while (GLenum error = glGetError()) {
				std::cout << "Error in function: " << call << std::endl;
				std::cout << "in file: " << file << std::endl;
				std::cout << "in line: " << line << std::endl;
				switch (error) {
				case GL_INVALID_OPERATION:
					std::cout << "OPENGL Invalid operation!: " << error << std::endl;
					break;
				case GL_INVALID_ENUM:
					std::cout << "OPENGL Invalid enum!: " << error << std::endl;
					break;
				case GL_INVALID_VALUE:
					std::cout << "OPENGL Invalid value!: " << error << std::endl;
					break;
				case GL_STACK_OVERFLOW:
					std::cout << "OPENGL Stack overflow!: " << error << std::endl;
					break;
				case GL_STACK_UNDERFLOW:
					std::cout << "OPENGL Stack underflow!: " << error << std::endl;
					break;
				default:
					std::cout << "OPENGL unknown error!: " << error << std::endl;
					break;
				}
				return false;
			}
			return true;
		}
		inline void checkGLError() { printGLError("null", __FILE__, __LINE__); }
		void printShaderError(int shaderLocation);

		inline void logError(const char error[], errorSeverity severity) {
			switch (severity) {
			case errorSeverity::WARNING:
				std::cout << "[Warning]: " << error << std::endl;
				break;
			case errorSeverity::CRITICAL:
				std::cerr << "[ERROR]: " << error << std::endl;
				break;
			case errorSeverity::FATAL:
				std::cerr << "[FATAL]: " << error << std::endl;
				break;
			}
		}


		std::string ReadTextFile(const std::string& shaderPath);
		std::string StemFileName(const std::string& fileName);
		class PlyParser {
		public:
			PlyParser(const std::string& plyPath);
			~PlyParser() {};

			std::vector<std::array<float, 3>> parseVertices();
		private:
			unsigned int numVertices = 0;
			std::string path;
		};

	}
}