#include <GL/glew.h>

#include "libraries/LoadObject.h"
#include "libraries/VectorUtils3.h"
#include "light.h"

#define MAX_LIGHTS 2


GLuint fullProgram;


void initializeFullShader();
void drawFull(Model *m, mat4 cameraTransform, mat4 modelTransforms, mat4* shadowMapTransform, GLuint texture,
              FBOstruct** shadowMaps, struct StreetLight* light, int nrLights, vec3 cameraPosition);
