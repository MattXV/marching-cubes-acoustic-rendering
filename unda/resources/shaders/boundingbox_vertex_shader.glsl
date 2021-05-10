#version 330 core

in vec4 vertexPosition;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec2 out_uvCoords;

void main() {
	gl_Position = projection * view * model * vertexPosition;
}