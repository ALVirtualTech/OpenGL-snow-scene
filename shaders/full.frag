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


vec2 poissonDisk[4] =
	vec2[](vec2( -0.94201624,  -0.39906216 ),
	       vec2( 0.94558609,   -0.76890725 ),
	       vec2( -0.094184101, -0.92938870 ),
	       vec2( 0.34495938,    0.29387760 ));


float random(vec3 seed, int i) {
	vec4 seed4 = vec4(seed, i);
	float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
	return fract(sin(dot_product) * 43758.5453);
}


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

float getShadow(int index) {
	// Perform perspective division to get the actual texture position
	vec4 shadowCoordinateWdivide = lightSourceCoord[index] / lightSourceCoord[index].w;

	// Used to lower moire' pattern and self-shadowing
	// The optimal value here will vary with different GPU's depending on their Z buffer resolution.
	shadowCoordinateWdivide.x -= 0.00001;
	shadowCoordinateWdivide.y -= 0.00001;
	shadowCoordinateWdivide.z -= 0.00005;

	float shadow = 1.0; // 1.0 = no shadow
	for (int i = 0; i < 4; i++) {
		int randomIndex = int(4.0 * random(gl_FragCoord.xyy, i)) % 4;
		float distanceFromLight = texture(shadowMap[0], shadowCoordinateWdivide.st + poissonDisk[randomIndex] / 1000.0).x;
		distanceFromLight = (distanceFromLight-0.5) * 2.0;

		if (lightSourceCoord[index].w > 0.0)
			if (distanceFromLight < shadowCoordinateWdivide.z)
				shadow -= 0.001 / (shadowCoordinateWdivide.z - distanceFromLight);
	}
	shadow = clamp(shadow, 0.2, 1.0);
	return shadow;
}

void main() {
	vec3 normal = normalize(transpose(inverse(mat3(model))) * fragNormal);

	vec3 surfacePos = vec3(model * vec4(fragPosition, 1));
	vec4 surfaceColor = texture(material, fragTexCoord);
	vec3 surfaceToCamera = normalize(cameraPosition - surfacePos);

	outColor = vec4(0);
	for (int i = 0; i < nrLights; i++) {
		outColor += vec4(applyLight(light[i], surfaceColor.rgb, normal,
		                            surfacePos, surfaceToCamera, getShadow(i)),
		                 surfaceColor.a);
	}
	// Set the index of shadowMap to 1 and this code will work.
	outColor += texture(shadowMap[0], vec2(1, 2)).x / 1000.0;
}
