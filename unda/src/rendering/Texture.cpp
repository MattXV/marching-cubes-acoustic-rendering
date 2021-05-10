#include "Texture.h"


Texture::Texture(const std::string& f)
	: file(f)
{
	stbi_set_flip_vertically_on_load(0);
	textureData = stbi_load(file.c_str(), &width, &height, &channels, STBI_rgb_alpha);

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
	if (imageLoaded) stbi_image_free(textureData);
	GLCALL(glDeleteTextures(1, &textureId));
}

int Texture::generatePatch(std::pair<double, double> minUV, std::pair<double, double> maxUV, const std::string& filename, bool opaque, bool nonBlack)
{
	if (!imageLoaded) {
		UNDA_ERROR("No texture image loaded!");
	}
	int xMin = (int)std::floor(minUV.first * width);
    int xMax = (int)std::floor(maxUV.first * width);
	int yMin = (int)std::floor(minUV.second * height);
	int yMax = (int)std::floor(maxUV.second * height);

	if (xMin > xMax) unda::integerAddSwap(&xMin, &xMax);
	if (yMin > yMax) unda::integerAddSwap(&yMin, &yMax);

	if (xMin > width)  return -1;
	if (xMin < 0)      return -1;
	if (xMax > width)  return -1;
	if (xMax == xMin)  return -1;
	if (yMin > height) return -1;
	if (yMin < 0)      return -1;
	if (yMax > height) return -1;
	if (yMax == yMin)  return -1;

	size_t patchWidth = size_t(xMax) - size_t(xMin);
	size_t patchHeight = size_t(yMax) - size_t(yMin);
	size_t patchChannels = 4;

	std::vector<unsigned char> patchPixels;
	patchPixels.resize(patchWidth * patchHeight * patchChannels);

	long int alphaMean = static_cast<long int>(UCHAR_MAX), colourMean = 0;
	for (size_t row = 0; row < patchHeight; row++) {
		for (size_t column = 0; column < patchWidth; column++) {
			unsigned char* imageIndex = (*this)[std::make_pair(column + size_t(xMin), row + size_t(yMin))];
			
			for (int patchChannel = 0; patchChannel < patchChannels; patchChannel++) {
				unsigned char pixelValue = *(imageIndex + patchChannel);
				
				if (opaque && patchChannel == 3) {
					alphaMean = (alphaMean + static_cast<long int>(pixelValue)) / 2;
				}

				if (nonBlack && patchChannel != 3) {
					colourMean = (colourMean + static_cast<long int>(pixelValue)) / 2;
				}
				patchPixels[(row * patchWidth + column) * patchChannels + patchChannel] = pixelValue;
			}
		}
	}

	if (opaque && alphaMean < static_cast<long int>(UCHAR_MAX)) return -1;
	if (nonBlack && colourMean == 0) return -1;

	auto imagePath = std::filesystem::path(file);
	std::string fileName = outputPatchesPrefix + imagePath.stem().string() + "_" + filename + "_" + std::to_string(patchesGenerated++) + "_.png";
	int written = stbi_write_png(
		fileName.c_str(),
		static_cast<int>(patchWidth),
		static_cast<int>(patchHeight),
		static_cast<int>(patchChannels),
		(void*)&patchPixels[0],
		static_cast<int>(patchChannels * patchWidth));
	return written;
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