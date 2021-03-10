#include "Terrain.h"

namespace unda {
	std::array<float, 30 * 30 * 30> heightMapTerrain(const std::string& heightmapFile) {
		
		std::array<float, 30 * 30 * 30> scalarField{};

		stbi_set_flip_vertically_on_load(1);
		int width, height, channels;
		unsigned char* image = stbi_load(heightmapFile.c_str(), &width, &height, &channels, STBI_grey);


		const size_t surfaceWidth = 30, surfaceDepth = 30, surfaceHeight = 30;

		int surfaceX, surfaceZ, surfaceY;
		float scalar = 0.0f, heightValue;
		for (size_t i = 0; i < surfaceWidth; i++) {
			for (size_t j = 0; j < surfaceDepth; j++) {
				
				surfaceX = (size_t)floorf(((float)i / (float)surfaceWidth) * (float)width);
				surfaceZ = (size_t)floorf(((float)j / (float)surfaceWidth) * (float)height);

				heightValue = ((float)image[surfaceX * width + surfaceZ] / 255.0f) * 0.2f;

				for (size_t k = 0; k < surfaceHeight; k++) {
					scalarField[i * surfaceHeight * surfaceDepth + k * surfaceDepth + j] = (float)(((float)heightValue * (float)surfaceHeight) >= (float)k);
				}
			}
		}

		stbi_image_free(image);
		return scalarField;
	}
};
