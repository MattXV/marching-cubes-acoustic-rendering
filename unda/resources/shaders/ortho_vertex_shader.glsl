#version 430 core


layout(location = 0) in vec4 vertexPosition; 
layout(location = 1) in vec2 uvCoordinates;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec2 out_uv;

void main() {
	out_uv = uvCoordinates;
	gl_Position = projection * view * model * vertexPosition;
}