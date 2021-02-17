#version 330 core

in vec4 vertexPosition; 
in vec2 uv;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
out vec2 out_uv;

void main() 
{ 
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;
	out_uv = uv;
}