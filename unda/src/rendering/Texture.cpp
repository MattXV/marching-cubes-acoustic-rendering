#include "Texture.h"


Texture::Texture(const std::string& f)
	: file(f)
{
	stbi_set_flip_vertically_on_load(1);
	textureData = stbi_load(file.c_str(), &width, &height, &channels, STBI_rgb_alpha);

	if (!textureData) {
		std::cerr << "[ERROR] Could not read image file! File: " << file.c_str() << std::endl;
		return;
	}

	imageFormat = GL_RGBA8;
	glImageFormat = GL_RGBA;

	setTextureData();
	// Allocate texture in OpenGL
	stbi_image_free(textureData);
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
	unsigned char* textureArray = new unsigned char[newWidth * newHeight * newChannels];
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
	GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)textureData));

	GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
}