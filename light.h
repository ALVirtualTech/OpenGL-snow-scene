#ifndef LIGHT_H
#define LIGHT_H

#include "camera.h"
#include "./libraries/VectorUtils3.h"

struct Light {
	struct Camera camera;
	vec3 intensities;
	float ambientCoefficient;
	float coneAngle;
};

struct ShaderLight {
	vec3 position;
	vec3 intensities;
	vec3 coneDirection;
	float ambientCoefficient;
	float coneAngle;
};

struct StreetLight {
	struct Light lamp;
	Model* model;
	vec3 position;
};

struct ShaderLight getShaderLight(struct Light *light);

struct StreetLight createStreetLight(vec3 position);

#endif
