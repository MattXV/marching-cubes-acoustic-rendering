#pragma once

#include <array>
#include <math.h>
#include "../../externals/stb_image/stb_image.h"
#include <string>


namespace unda {
	std::array<float, 30 * 30 * 30> heightMapTerrain(const std::string& heightmapFile);
};
