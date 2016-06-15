#version 150

#define MAX_LIGHTS 2

in vec4 lightSourceCoord[MAX_LIGHTS];
in vec2 fragTexCoord;
in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 cameraPosition;
uniform mat4 model;
uniform sampler2D material;
uniform sampler2D normalMap;
uniform sampler2D shadowMap[MAX_LIGHTS];
uniform int nrLights;
uniform struct Light {
	vec3 position;
	vec3 intensities;
	float ambientCoefficient;
	float coneAngle;
	vec3 coneDirection;
} light[MAX_LIGHTS];

out vec4 outColor;


vec3 applyLight(Light light, vec3 surfaceColor, vec3 normal, vec3 surfacePos, vec3 surfaceToCamera, float shadow) {
	float materialShininess = 1.0;
	vec3 materialSpecularColor = vec3(0.25);

	vec3 surfaceToLight = normalize(light.position - surfacePos);
	float distanceToLight = length(light.position - surfacePos);
	float attenuation = 1.0 / (0.1 + 0.0001 * pow(distanceToLight, 3));

	// Cone restrictions
	float lightToSurfaceAngle = degrees(acos(dot(-surfaceToLight, normalize(light.coneDirection))));
	if(lightToSurfaceAngle > light.coneAngle / 2) {
		attenuation = 0.0;
	}

	float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight) * 0.5f + 0.5f);
	float specularCoefficient = diffuseCoefficient > 0 ?
		pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), materialShininess) : 0;

	vec3 ambient = light.ambientCoefficient * surfaceColor.rgb * light.intensities;
	vec3 diffuse = diffuseCoefficient * surfaceColor.rgb * light.intensities;
	vec3 specular = specularCoefficient * materialSpecularColor * light.intensities;

	return ambient + shadow * attenuation * (diffuse + specular);
}


void main() {
	vec3 normal = normalize(transpose(inverse(mat3(model))) * fragNormal);

	vec3 surfacePos = vec3(model * vec4(fragPosition, 1));
	vec4 surfaceColor = texture(material, fragTexCoord);
	vec3 surfaceToCamera = normalize(cameraPosition - surfacePos);

	outColor = vec4(0);
	for (int i = 0; i < nrLights; i++) {
		outColor += vec4(applyLight(light[i], surfaceColor.rgb, normal,
		                            surfacePos, surfaceToCamera, 1.0),
		                 surfaceColor.a);
	}
	vec4 shadowCoordinateWdivide = lightSourceCoord[1] / lightSourceCoord[1].w;
	// Set the index of shadowMap to 1 and this code will work.
	outColor += texture(shadowMap[1], shadowCoordinateWdivide.st + vec2(1.0, 2.0) / 1000.0).x / 10;
}
