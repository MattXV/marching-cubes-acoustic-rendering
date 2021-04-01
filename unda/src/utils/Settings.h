#pragma once

namespace unda {
		static int windowWidth = 1280;
		static int windowHeight = 1024;
		static double sampleRate = 48000;

	namespace shaders {
		const static std::string vertexShaderSource = "resources/shaders/vertex_shader.glsl";
		const static std::string fragmentShaderSource = "resources/shaders/fragment_shader.glsl";
		const static std::string lightVertexShaderSource = "resources/shaders/lightsource_vertex_shader.glsl";
		const static std::string lightFragmentShaderSource = "resources/shaders/lightsource_fragment_shader.glsl";

		const static int vertexPositionLocation = 0;
		const static int uvCoordinatesLocation = 1;
		const static int vertexNormalLocation = 2;

		const static int lightVertexPositionLocation = 0;
		const static int lightVertexNormalLocation = 1;
	}
	
	static bool vSync = true;
}