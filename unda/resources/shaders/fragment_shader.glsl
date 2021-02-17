#version 330 core

out vec4 fragmentColor;
in vec2 out_uv;

uniform sampler2D textureSampler;

void main() 
{ 
	fragmentColor = texture(textureSampler, out_uv);
}