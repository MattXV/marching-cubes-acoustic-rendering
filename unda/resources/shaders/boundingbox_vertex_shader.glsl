#version 330 core

in vec4 vertexPosition;
in vec3 cubeUVs;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform vec3 cubeSize;

out vec3 out_cubeUVCoords;


void main() {
	gl_Position = projection * view * model * vertexPosition;
	vec4 bbPosition = model * vertexPosition;
	out_cubeUVCoords = normalize(vec3((bbPosition.x) , (bbPosition.y) , (bbPosition.z) ));

}