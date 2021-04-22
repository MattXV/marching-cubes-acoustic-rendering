#pragma once

#include "../rendering/RenderTools.h"
#include "../utils/Utils.h"
#include <stb_image.h>
#include <stb_image_write.h>
#include <string>

class Texture {
public:
	Texture(const std::string& fileName);
	Texture(const int textureHeight, const int textureWidth, unda::Colour<unsigned char> colour);
	~Texture();
	unsigned int getTextureId() { return textureId; }
	int generatePatch(std::pair<double, double> minUV, std::pair<double, double> maxUV);


	unsigned char* operator[](const std::pair<size_t, size_t> xy);
private:
	void setTextureData();

	bool imageLoaded = false;
	int width, height, channels;
	unsigned char* textureData;
	std::string file;
	std::string outputPatchesPrefix = "output/patches";
	int patchesGenerated = 0;

	// OpenGL
	unsigned int textureId = 0;
	int imageFormat = GL_RGBA8;
	int glImageFormat = GL_RGBA;
};