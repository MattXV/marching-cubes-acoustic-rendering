#version 330 core

out vec4 fragmentColour;

uniform vec3 lightColour;


void main() {
    fragmentColour = vec4(lightColour, 1.0f);
}