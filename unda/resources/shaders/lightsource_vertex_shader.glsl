#version 330 core

layout(location = 0) in vec4 vertexPosition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;
}