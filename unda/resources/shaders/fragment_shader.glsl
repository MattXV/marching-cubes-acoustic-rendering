#version 330 core


in vec2 out_uv;
in vec3 fragPosition;
in vec3 out_normal;
out vec4 fragmentColor;

uniform sampler2D textureSampler;
uniform vec3 lightColour;
uniform vec3 lightPosition;
uniform vec3 viewPosition;


void main() {
	// Ambient Light
	float ambientStrength = 0.4f;
	vec3 ambient = ambientStrength * lightColour;
	
	vec3 normal = normalize(out_normal);

	vec3 lightDirection = normalize(lightPosition - fragPosition);
	float diff = max(dot(normal, lightDirection), 0.0f);
	vec3 diffuse = diff * lightColour;

	// Specular Lighting
	float specularStrength = 0.2f;
	vec3 viewDir = normalize(viewPosition - fragPosition);
	vec3 reflectDirection = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDir, reflectDirection), 0.0f), 32);
	vec3 specular = specularStrength * spec * lightColour;
	
	// Texture
	vec3 textureColour = texture(textureSampler, out_uv).xyz;
	
	// Combining them together
	vec3 result = (ambient + diffuse) * textureColour + specular;

	fragmentColor = vec4(result, 1.0f);
};