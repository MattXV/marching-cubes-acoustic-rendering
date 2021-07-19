#version 330 core

in vec3 out_cubeUVCoords;

uniform samplerCube cubeMap;

out vec4 fragmentColour;

void main() {
	fragmentColour = vec4(texture(cubeMap, out_cubeUVCoords).rgb, 1.0f);
	//fragmentColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}