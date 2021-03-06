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


unda::utils::PlyParser::PlyParser(const std::string& plyPath)
{
	std::ifstream file;
	std::stringstream fileStream;
	std::string line;

	file.open(plyPath);

	for (int i = 0; i < 100; i++) {
		std::getline(fileStream, line);
		std::cout << line << std::endl;
	}

	std::string delimiter = " ";
	if (file.is_open()) 
		while (std::getline(fileStream, line)) {
			if (line.find("ply") == std::string::npos && !line.empty()) {
				std::cerr << "Error parsing PLY!, Header doesn't seem to start with 'ply'..." << std::endl;
				file.close();
				return;
			}

		while (std::getline(fileStream, line) || line.find("end_header") == std::string::npos) {
			
			if (line.find("element") != std::string::npos) {
				line.erase(0, line.find(delimiter) + delimiter.length());
				if (line.find("vertex") != std::string::npos) {
					line.erase(0, line.find(delimiter) + delimiter.length());
					numVertices = (unsigned int)std::stoi(line);
				}
			}
		
		}

	}
	else {
		printf("[ERROR] Could not open file: %s \n", plyPath.c_str());
	}
	if (numVertices == 0) {
		std::cerr << "Error parsing PLY!, Could not find 'element vertex'..." << std::endl;
	}
}

std::vector<std::array<float, 3>> unda::utils::PlyParser::parseVertices()
{
	std::vector<std::array<float, 3>> vertices;

	std::ifstream file;
	std::stringstream fileStream;
	std::string line;

	std::string delimiter = " ", token;
	size_t pos = 0;
	size_t lastNumElements = 0;
	if (file.is_open()) {
		while (std::getline(fileStream, line) || line.find("end_header") == std::string::npos);
		while (std::getline(fileStream, line)) {
			size_t NumElements = line.rfind(delimiter);
			if (lastNumElements != 0)
			if ((NumElements != lastNumElements))
				break;

			std::array<float, 3U> vertex = { 0.0f, 0.0f, 0.0f };
			
			for (int i = 0; i < 3; i++) {
				pos = line.find(delimiter);
				float vertexValue = std::stof(line.substr(0, pos));
				vertex[i] = vertexValue;
				line.erase(0, pos + delimiter.length());
			}
			vertices.push_back(vertex);
		}
	}
	else {
		printf("[ERROR] Could not open file: %s \n", path.c_str());
	}

	return vertices;
}