#pragma once

#include "../rendering/RenderTools.h"
#include "../utils/Utils.h"
#include "../utils/Maths.h"
#include <glob.h>
#include <vector>
#include <stb_image.h>
#include <stb_image_write.h>
#include <string>
#include <filesystem>
#include <limits>
#include <algorithm>




class Texture;

struct TexturePatch {
	Texture* parentTexture;
	std::vector<unsigned char> pixels = std::vector<unsigned char>();
	int height, width, channels;
	int cubeMapFace = 0;
};


class Texture {
public:
	Texture(const std::string& fileName);
	Texture(const int textureHeight, const int textureWidth, unda::Colour<unsigned char> colour);
	~Texture();
	unsigned int getTextureId() { return textureId; }
	bool generatePatch(std::pair<double, double> minUV, std::pair<double, double> maxUV, const std::string& filename, TexturePatch& outPatch, bool opaque = true, bool nonBlack = true);
	bool isImageLoaded() const { return imageLoaded; }

	unsigned char* operator[](const std::pair<size_t, size_t> xy);

private:
	void setTextureData();

	bool imageLoaded = false;
	int width, height, channels;
	unsigned char* textureData;
	std::string file;
	std::string outputPatchesPrefix = "output/patches/";
	int patchesGenerated = 0;

	// OpenGL
	unsigned int textureId = 0;
	int imageFormat = GL_RGBA8;
	int glImageFormat = GL_RGBA;
};

class CubeMap {
public:
	CubeMap();
	~CubeMap();

	enum Face {
		POSITIVE_X = 0x8515,
		NEGATIVE_X = 0x8516,
		POSITIVE_Y = 0x8517,
		NEGATIVE_Y = 0x8518,
		POSITIVE_Z = 0x8519,
		NEGATIVE_Z = 0x851A
	};

	unsigned int getTextureLocation() { return textureLocation; }
	void applyPatch(TexturePatch& patch, Face face);
	
	std::vector<unsigned char> whitePixels;
	const size_t faceSize = 64;
	const size_t cubemapSize = static_cast<size_t>(pow(64, 2) * 3);

private:
	unsigned int textureLocation = 0;
};