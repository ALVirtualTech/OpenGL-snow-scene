#version 150
#extension GL_ARB_explicit_attrib_location : enable

#define MAX_LIGHTS 2

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoordinates;

uniform mat4 shadowMapTransform[MAX_LIGHTS];
uniform mat4 camera;
uniform mat4 model;
uniform int nrLights;

out vec4 lightSourceCoord[MAX_LIGHTS];
out vec2 fragTexCoord;
out vec3 fragPosition;
out vec3 fragNormal;

void main() {
	// Transform vertex to light source coordinates
	for (int i = 0; i < nrLights; i++)
		lightSourceCoord[i] = shadowMapTransform[i] * vec4(inPosition, 1.0);

	gl_Position = camera * model * vec4(inPosition, 1.0);
	// Pass to fragment shader
	fragTexCoord = textureCoordinates;
	fragPosition = inPosition;
	fragNormal = normal;
}
