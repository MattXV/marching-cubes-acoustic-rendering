#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../externals/stb_image/stb_image.h"

// Modules
#include "acoustics/ImageSource.h"

#include "core/Engine.h"

#include "input/IEventHandler.h"
#include "input/GLFWApplication.h"

#include "rendering/MarchingCubes.h"
#include "rendering/ModelRenderer.h"
#include "rendering/RenderTools.h"
#include "rendering/Texture.h"


#include "utils/Utils.h"
#include "utils/Settings.h"

#include "scene/Model.h"