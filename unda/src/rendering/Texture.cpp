#include "Texture.h"


Texture::Texture(const std::string& f)
	: file(f)
{
	stbi_set_flip_vertically_on_load(0);
	textureData = stbi_load(file.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	channels = STBI_rgb_alpha;
	if (!textureData) {
		UNDA_ERROR("Could not Read texture! File: " + f);
		return;
	}
	imageLoaded = true;
	imageFormat = GL_RGBA8;
	glImageFormat = GL_RGBA;

	setTextureData();
	// Allocate texture in OpenGL
}

Texture::~Texture()
{
	if (imageLoaded && textureData) stbi_image_free(textureData);
	GLCALL(glDeleteTextures(1, &textureId));
}

bool Texture::generatePatch(std::pair<double, double> minUV, std::pair<double, double> maxUV, const std::string& filename, TexturePatch& outPatch, bool opaque, bool nonBlack)
{
	const int threshold = 127;
	if (!imageLoaded) {
		return false;
	}
	int xMin = (int)std::floor(minUV.first * width);
    int xMax = (int)std::floor(maxUV.first * width);
	int yMin = (int)std::floor(minUV.second * height);
	int yMax = (int)std::floor(maxUV.second * height);

	if (xMin > xMax) unda::integerAddSwap(&xMin, &xMax);
	if (yMin > yMax) unda::integerAddSwap(&yMin, &yMax);

	if (xMin > width)  return false;
	if (xMin < 0)      return false;
	if (xMax > width)  return false;
	if (xMax == xMin)  return false;
	if (yMin > height) return false;
	if (yMin < 0)      return false;
	if (yMax > height) return false;
	if (yMax == yMin)  return false;

	size_t patchWidth = size_t(xMax) - size_t(xMin);
	size_t patchHeight = size_t(yMax) - size_t(yMin);
	const size_t patchChannels = 3;

	std::vector<unsigned char> patchPixels;
	patchPixels.resize(patchWidth * patchHeight * patchChannels);

	int alphaMean = static_cast<int>(UCHAR_MAX), colourMean = 0;
	for (size_t row = 0; row < patchHeight; row++) {
		for (size_t column = 0; column < patchWidth; column++) {
			unsigned char* imageIndex = (*this)[std::make_pair(column + size_t(xMin), row + size_t(yMin))];
			
			for (int patchChannel = 0; patchChannel < patchChannels; patchChannel++) {
				unsigned char pixelValue = *imageIndex++;
				if (opaque && patchChannel == 3) {
					alphaMean = (alphaMean + static_cast<int>(pixelValue)) / 2;
				}
				if (nonBlack && patchChannel != 3) {
					colourMean = (colourMean + static_cast<int>(pixelValue)) / 2;
				}
				patchPixels[(row * patchWidth + column) * patchChannels + patchChannel] = pixelValue;
			}
		}
	}

	//if (opaque && alphaMean < threshold) return false;
	if (nonBlack && colourMean == 0) return false;

	auto imagePath = std::filesystem::path(file);
	std::string fileName = outputPatchesPrefix + filename + ".png";
	stbi_write_png_compression_level = 0;
	int written;
	written = stbi_write_png(
		fileName.c_str(),
		(int)(patchWidth),
		(int)(patchHeight),
		(int)(patchChannels),
		(void*)&patchPixels[0],
		(int)(patchChannels * patchWidth * sizeof(unsigned char)));
	if (written == 0) return false;
	
	outPatch.cubeMapFace = 
	outPatch.height = patchHeight;
	outPatch.width = patchWidth;
	outPatch.channels = patchChannels;
	outPatch.pixels = std::vector(std::move(patchPixels));
	outPatch.parentTexture = this;


	return true;
}

unsigned char* Texture::operator[](const std::pair<size_t, size_t> xy)
{
	assert(xy.first >= 0 && xy.first <= width);
	assert(xy.second >= 0 && xy.second <= height);

	//unsigned char* pointer = &textureData[(xy.second * size_t(width) + xy.first) * size_t(channels)];

	unsigned char* pointer = &textureData[(xy.second * size_t(width) + xy.first) * size_t(channels)];

	return pointer;
}

Texture::Texture(const int textureHeight, const int textureWidth, unda::Colour<unsigned char> colour)
{
	const int newHeight = textureHeight;
	const int newWidth = textureWidth;
	const int newChannels = 4;
	width = newWidth;
	height = newHeight;
	channels = newChannels;
	imageFormat = GL_RGBA8;
	glImageFormat = GL_RGBA;
	// Create an array of pixel data.
	unsigned char* textureArray = new unsigned char[(size_t)newWidth * (size_t)newHeight * (size_t)newChannels];
	for (int i = 0; i < newWidth * newHeight; i++) {
		textureArray[i * channels] = colour.r;       // Red
		textureArray[i * channels + 1] = colour.g;   // Green
		textureArray[i * channels + 2] = colour.b;   // Blue
		textureArray[i * channels + 3] = colour.a;   // Alpha 
	}
	textureData = textureArray;

	// Set white pixels to the GL Texture.
	setTextureData();
	delete[] textureArray;
}


void Texture::setTextureData()
{
	GLCALL(glGenTextures(1, &textureId));
	GLCALL(glBindTexture(GL_TEXTURE_2D, textureId));

	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)); // Set how the texture should upscale
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));  // and downscale.
	GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)textureData));

	GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
}


CubeMap::CubeMap() {
	GLCALL(glGenTextures(1, &textureLocation));
	GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, textureLocation));

	std::filesystem::path defaultTextures = std::filesystem::path("resources/textures/cubemap/");

	
	whitePixels.assign(cubemapSize, 255);

	stbi_set_flip_vertically_on_load(0);
	for (int face = 0; face < 6; face++) {
		auto faceTexture = defaultTextures;
		faceTexture = faceTexture.append(std::to_string(face) + ".png");
		std::string path = faceTexture.string();
		int width, height, channels;
		unsigned char* image = stbi_load((char*)path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		
		//GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)image));
		//GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB8, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)whitePixels.data()));
		stbi_image_free(image);
	}

	GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

	GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, NULL));
}

CubeMap::~CubeMap() {
	GLCALL(glDeleteTextures(1, &textureLocation));
}

void CubeMap::applyPatch(TexturePatch& patch, Face face)
{
	while (GLenum error = glGetError()) {
		__debugbreak();
	}
	int square = std::min<int>(patch.width, patch.height);
	
	// crop to square
	unsigned char* squarePatch = new unsigned char[cubemapSize];
	unsigned char blackPixel[] = { 0, 0, 0 };

	for (size_t row = 0; row < faceSize; row++) {
		for (size_t column = 0; column < faceSize; column++) {
			unsigned char* pixel;
			if (row < patch.height && column < patch.width)
				pixel = &patch.pixels[(row * patch.width + column) * patch.channels];
			else
				pixel = blackPixel;
			for (int channel = 0; channel < 3; channel++)
				squarePatch[(row * faceSize + column) * 3 + channel] = (unsigned char)(*pixel++);
		}
	} 

	GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, textureLocation));

		GLCALL(glTexImage2D(face, 0, GL_RGB8, faceSize, faceSize, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)squarePatch));

	GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, NULL));
	delete[] squarePatch;
}