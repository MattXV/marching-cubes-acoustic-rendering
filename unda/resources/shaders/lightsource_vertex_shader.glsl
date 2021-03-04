#version 330 core

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragmentPosition;

void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;
	fragmentPosition = vec3(modelMatrix * vertexPosition);
}