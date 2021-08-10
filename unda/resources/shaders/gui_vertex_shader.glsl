#version 430 core

in vec3 vertexPosition;
in vec2 vertexUVs;

uniform mat4 projection;

out vec2 outUVs;

void main() {
	gl_Position = projection * vec4(vertexPosition.xy, 0.0f, 1.0f);
	outUVs = vertexUVs;
}