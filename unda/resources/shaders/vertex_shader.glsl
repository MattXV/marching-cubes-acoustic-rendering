#version 330 core

in vec4 vertexPosition; 
in vec2 uvCoordinates;
in vec3 vertexNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 out_uv;
out vec3 out_normal;
out vec3 fragPosition;

void main() 
{ 
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;
	out_uv = uvCoordinates;
	out_normal = vertexNormal;
	fragPosition = vec3(modelMatrix * vertexPosition);
};