#include "Utils.h"



std::string unda::utils::ReadTextFile(const std::string& path)
{
	std::ifstream shaderFile(path);
	std::stringstream shaderSource;

	std::string line;
	if (shaderFile.is_open()) {
		while (std::getline(shaderFile, line)) {
			shaderSource << line << std::endl;
		}
		shaderFile.close();
	}
	else {
		printf("[ERROR] Could not open file: %s \n", path.c_str());
	}
	return shaderSource.str();
}

std::string unda::utils::StemFileName(const std::string& fileName)
{
	const rsize_t pos = fileName.find_last_of('.');
	std::string stem = fileName.substr(0, pos);
	return stem;
}

void unda::utils::printShaderError(int shaderLocation)
{
	int logLength;
	glGetShaderiv(shaderLocation, GL_INFO_LOG_LENGTH, &logLength);
	char* errorMessage = new char[(long long)logLength + 1];
	glGetShaderInfoLog(shaderLocation, logLength, &logLength, &errorMessage[0]);
	std::cout << "[GLSL Error]: Could not compile shader." << std::endl;
	std::cout << errorMessage << std::endl;
	delete[] errorMessage;
}
