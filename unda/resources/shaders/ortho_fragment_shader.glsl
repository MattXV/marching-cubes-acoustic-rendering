#version 430 core


in vec2 out_uv;

uniform sampler2D textureSampler;

out vec4 fragmentColour;

void main() {
	// Texture
	fragmentColour = texture(textureSampler, out_uv).rgba;
};