#include <GL/glew.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "libraries/LoadTGA.h"
#include "libraries/VectorUtils3.h"
#include "libraries/GLUtilities.h"
#include "libraries/LoadObject.h"

#include "main.h"
#include "instancing.h"
#include "ground.h"
#include "camera.h"
#include "shadow.h"
#include "content.h"

#define TEX_UNIT 0
#define FBO_RES 2048


struct Camera userCamera;
struct Camera pointLight;

GLuint fullProgram, plainProgram, instancingProgram;
GLuint projTexMapUniform;

FBOstruct *fbo;

mat4 modelViewMatrix, textureMatrix;
mat4 projectionMatrix;
mat4 transCubes;

void reshapeViewport(GLsizei w, GLsizei h) {
	glViewport(0, 0, w, h);
	userCamera.projection = perspective(90, (GLfloat)w/(GLfloat)h, 0.1, 1000);
}

void initUserCamera() {
	vec3 position = (vec3){1.5f, 20.0f, -50.0f};
	vec3 normal = (vec3){0.0f, 1.0f, 0.0f};
	vec3 target = (vec3){10.0f, 15.0f, 5.0f};
	userCamera = createUserCamera(position, normal, target, 90.0);
}


void initpointLight() {
	vec3 position = (vec3){40, 20, 0};
	vec3 target = (vec3){0, 3, -10};
	vec3 normal = CrossProduct(position, target);
	pointLight = createUserCamera(position, normal, target, 90.0);
}


void loadShadowShader() {
	fullProgram = loadShaders("shaders/full.vert", "shaders/full.frag");
	projTexMapUniform = glGetUniformLocation(fullProgram,"textureUnit");
	plainProgram = loadShaders("shaders/plain.vert", "shaders/plain.frag");
	instancingProgram = loadShaders("./shaders/instancing.vert", "./shaders/instancing.frag");
}


void rotateLight(void) {
	pointLight.position.x = 30.0 * -cos(glutGet(GLUT_ELAPSED_TIME)/10000.0);
	pointLight.position.z = 30.0 * -sin(glutGet(GLUT_ELAPSED_TIME)/10000.0);
}


// Build the transformation sequence for the light source path,
// by copying from the ordinary camera matrices.
void setTextureMatrix(void) {
	// Scale and bias transform, moving from unit cube [-1,1] to [0,1]
	mat4 scaleBiasMatrix = Mult(T(0.5, 0.5, 0.0), S(0.5, 0.5, 1.0));
	textureMatrix = Mult(Mult(scaleBiasMatrix, userCamera.projection), modelViewMatrix);
}


void loadObjects(void) {
	transCubes = T(-10, 100, -10);
}

void drawObjects() {
  mat4 mv2, tx2, trans;

	glUniformMatrix4fv(glGetUniformLocation(fullProgram, "projectionMatrix"), 1, GL_TRUE, userCamera.projection.m);

	// Ground
	glUniform1f(glGetUniformLocation(fullProgram, "shade"), 0.3); // Dark ground
	glUniformMatrix4fv(glGetUniformLocation(fullProgram, "modelViewMatrix"), 1, GL_TRUE, modelViewMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(fullProgram, "textureMatrix"), 1, GL_TRUE, textureMatrix.m);
	drawGroundWithProgram(fullProgram);

	glUniform1f(glGetUniformLocation(fullProgram, "shade"), 0.9); // Brighter objects

	// The sphere
	trans = Mult(T(0,4,-5), S(5.0, 5.0, 5.0));
	mv2 = Mult(modelViewMatrix, trans); // Apply on both
	tx2 = Mult(textureMatrix, trans);
	// Upload both!
	glUniformMatrix4fv(glGetUniformLocation(fullProgram, "modelViewMatrix"), 1, GL_TRUE, mv2.m);
	glUniformMatrix4fv(glGetUniformLocation(fullProgram, "textureMatrix"), 1, GL_TRUE, tx2.m);
	DrawModel(modelCube, fullProgram, "in_Position", NULL, NULL);
}

void renderScene(void) {
	rotateLight();
	userCamera = moveCameraOnKeyboard(userCamera);
	mat4 projectionViewMatrix = getProjectionViewMatrix(userCamera);

	// Setup the modelview from the light source
	modelViewMatrix = lookAtv(pointLight.position, pointLight.target, pointLight.normal);
	// Using the result from lookAt, add a bias to position the result properly in texture space
	setTextureMatrix();

	// 1. Render scene to FBO
	useFBO(fbo, NULL, NULL);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Depth only
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Using the simple shader
	glUseProgram(plainProgram);
	glUniform1i(projTexMapUniform,TEX_UNIT);
	glActiveTexture(GL_TEXTURE0 + TEX_UNIT);
	glBindTexture(GL_TEXTURE_2D,0);
	drawObjects();
	printError("Draw me like one of your french girls");

	mat4 trans = Mult(T(0,4,-16), S(2.0, 2.0, 2.0)); // Apply on both
	projectionViewMatrix = Mult(userCamera.projection, trans);
	drawModelInstanced(modelCube, instancingProgram, transCubes, projectionViewMatrix);
	glFlush();

	//2. Render from camera.
	useFBO(NULL, fbo, NULL);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(fullProgram);
	glUniform1i(projTexMapUniform,TEX_UNIT);
	glActiveTexture(GL_TEXTURE0 + TEX_UNIT);
	glBindTexture(GL_TEXTURE_2D,fbo->depth);

	modelViewMatrix = lookAtv(userCamera.position, userCamera.target, userCamera.normal);

	glCullFace(GL_BACK);
	drawObjects();

	trans = Mult(T(0,4,-5), S(5.0, 5.0, 5.0));
	projectionViewMatrix = getProjectionViewMatrix(userCamera);
	drawModelInstanced(modelCube, instancingProgram, transCubes, projectionViewMatrix);

	glutSwapBuffers();
}


void handleMouse(int x, int y) {
	userCamera = rotateCameraByMouse(userCamera, x, y);
}


void onTimer(int value) {
	glutPostRedisplay();
	glutTimerFunc(16, &onTimer, value);
	printError("OnTimer()");
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 1);
	glutCreateWindow("Shadow mapping demo");
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

	loadShadowShader();
	loadContent();
	loadObjects();
	initUserCamera();
	initpointLight();
	initKeymapManager();
	setupInstancedVertexAttributes(instancingProgram, 10);
	fbo = initFBO2(FBO_RES, FBO_RES, 0, 1);
	initializeGround(modelPlane, fullProgram);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0,0,0,1.0f);
	glEnable(GL_CULL_FACE);

	glutDisplayFunc(renderScene);
	glutTimerFunc(16, &onTimer, 0);

	glutMainLoop();
}
