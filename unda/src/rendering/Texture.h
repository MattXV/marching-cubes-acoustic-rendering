#pragma once

#include "../rendering/RenderTools.h"
#include "../unda.h"

class Texture {
public:
	Texture(const std::string& fileName);
	Texture(const int textureHeight, const int textureWidth, unda::Colour<unsigned char> colour);

	
private:
	void setTextureData();

	int width, height, channels;
	unsigned char* textureData;
	std::string file;
	unsigned int textureId = 0;
	int imageFormat = GL_RGBA8;
	int glImageFormat = GL_RGBA;
};