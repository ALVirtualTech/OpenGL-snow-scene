#include <GL/glew.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "libraries/VectorUtils3.h"
#include "libraries/GLUtilities.h"
#include "libraries/LoadObject.h"

#include "main.h"
#include "instancing.h"
#include "content.h"
#include "skybox.h"
#include "full.h"
#include "plain.h"
#include "simple.h"


#define FBO_RES 2048
#define NR_STREET_LIGHTS 2


vec3 snowPosRelativeLight;

int displayFBO = 0;
int displayFBOKeyWasDown = 0;

struct StreetLight lights[NR_STREET_LIGHTS];
FBOstruct* fbos[NR_STREET_LIGHTS];


void reshapeViewport(GLsizei w, GLsizei h) {
	glViewport(0, 0, w, h);
	userCamera.base.projection = perspective(90, 1.0 * w / h, 0.1, 1000);
}


void initUserCamera() {
	vec3 position = (vec3){40, 20, 0};
	vec3 target = (vec3){0, 3, -10};
	vec3 normal = (vec3){0, 1, 0};
	userCamera = createShakeableCamera(position, normal, target);
	userCamera.base.projection = perspective(90, (GLfloat)glutGet(GLUT_WINDOW_X) / (GLfloat)glutGet(GLUT_WINDOW_Y), 0.1, 1000);
}


void initStreetLights() {
	lights[0] = createStreetLight((vec3){0.0, 0.0, 0.0});
	lights[1] = createStreetLight((vec3){-80.0, 0.1, -80.0});
}


void initShaders() {
	initializeFullShader();
	initializeSimpleShader();
	initializePlainShader();
	initializeInstancingShader(15);
	initializeSkyboxShader();
}


mat4 getShadowMapTransform(mat4 modelViewProjectionTransform) {
	// Scale and bias transform, moving from unit cube [-1,1] to [0,1]
	mat4 scaleBiasMatrix = Mult(T(0.5, 0.5, 0.0), S(0.5, 0.5, 1.0));
	return Mult(scaleBiasMatrix, modelViewProjectionTransform);
}


void renderScene(void) {
	// Quit when Esc is pressed.
	if(keyIsDown(27)) {
		glutLeaveMainLoop();
	}

	updateCamera(&userCamera);

	mat4 cameraTransform = getProjectionViewMatrix((struct Camera *) &userCamera);
	mat4 lightTransforms[NR_STREET_LIGHTS];
	mat4 shadowMapTransforms[NR_STREET_LIGHTS];
	for (unsigned int i = 0; i < NR_STREET_LIGHTS; i++) {
		lightTransforms[i] = getProjectionViewMatrix((struct Camera *) &lights[i]);
		shadowMapTransforms[i] = getShadowMapTransform(lightTransforms[i]);
	}

	// 1. Render scene to FBO
	for (unsigned int i = 0; i < NR_STREET_LIGHTS; i++) {
		useFBO(fbos[i], NULL, NULL);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	for (unsigned int i = 0; i < NR_STREET_LIGHTS; i++) {
		useFBO(fbos[i], NULL, NULL);
		drawPlain(modelLightPost, lightTransforms[i], Tvec3(lights[i].position));
		drawModelInstanced(modelCube, lightTransforms[i], Tvec3(VectorAdd(lights[i].position, snowPosRelativeLight)), &lights[i].lamp);
		printError("Draw me like one of your french girls");
	}

	// 2. Render from camera.
	for (unsigned int i = 0; i < NR_STREET_LIGHTS; i++) {
		useFBO(NULL, fbos[i], NULL);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	drawSkybox(cameraTransform);
	for (unsigned int i = 0; i < NR_STREET_LIGHTS; i++) {
		useFBO(NULL, fbos[i], NULL);
		drawModelInstanced(modelCube, cameraTransform, Tvec3(VectorAdd(lights[i].position, snowPosRelativeLight)), &lights[i].lamp);
		drawFull(modelLightPost, cameraTransform, Tvec3(lights[i].position), shadowMapTransforms[i], textureMetal,
				 fbos[i]->depth, &lights[i].lamp, userCamera.base.position);
		drawFull(modelPlane, cameraTransform, Mult(Tvec3(lights[i].position), T(0.0, 0.0, -15.0)), shadowMapTransforms[i], textureGroundDiffuse,
				 fbos[i]->depth, &lights[i].lamp, userCamera.base.position);
		printError("Draw me like one of your italian girls");
	}
	drawFull(modelPlane, cameraTransform, Mult(S(10.0, 10.0, 10.0), T(0.0, -0.1, 0.0)), shadowMapTransforms[0],
					 textureGroundDiffuse, fbos[0]->depth, &lights[0].lamp, userCamera.base.position);


	// Toggle display FBO with 'f'.
	int displayFBOKeyIsDown = keyIsDown('f');
	if(displayFBOKeyWasDown && !displayFBOKeyIsDown) {
		displayFBO = (displayFBO + 1) % (NR_STREET_LIGHTS + 1);
	}
	displayFBOKeyWasDown = displayFBOKeyIsDown;
	if(displayFBO) {
		drawSimple(modelPlane, Mult(S(.04, .04, .04), Rx(45)), IdentityMatrix(), fbos[displayFBO - 1]->depth);
		printError("FBO me timbers!");
	}

	glutSwapBuffers();
}


void handleMouse(int x, int y) {
	updateCameraByMouse((struct Camera *) &userCamera, x, y);
}


void onTimer(int value) {
	printError("Här var det error upp över öronen!");
	glutPostRedisplay();
	glutTimerFunc(16, &onTimer, value);
	printError("OnTimer()");
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 1);
	glutCreateWindow("Let it Snow!");
	glutPassiveMotionFunc(handleMouse);
	glutReshapeFunc(reshapeViewport);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	// https://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW_.28OpenGL_Extension_Wrangler.29
	printError ("It should be safe to ignore this, see comment in code.");
	if (GLEW_OK != err) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	dumpInfo();

	for (unsigned int i = 0; i < NR_STREET_LIGHTS; i++)
		fbos[i] = initFBO2(FBO_RES, FBO_RES, 0, 1);

	snowPosRelativeLight = (vec3){0.0f, 30.0f, -10.5f};

	initShaders();
	loadContent();
	initUserCamera();
	initStreetLights();
	initKeymapManager();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0,0,0,1.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glutDisplayFunc(renderScene);
	glutTimerFunc(16, &onTimer, 0);

	glutMainLoop();
}
