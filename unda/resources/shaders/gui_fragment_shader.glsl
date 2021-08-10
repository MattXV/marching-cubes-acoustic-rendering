#version 430 core

in vec2 outUVs;

uniform sampler2D imageTexture;

out vec4 fragmentColour;

void main() {
	fragmentColour = texture(imageTexture, vec2(outUVs.x, 1.0f - outUVs.y)).rgba;
}