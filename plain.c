#include <GL/glew.h>

#include "plain.h"
#include "utilities.h"


static GLuint positionLocation = 0;

static GLuint modelViewProjectionLocation;
static GLuint shadeLocation;


void initializePlainShader(GLuint texUnit) {
	plainProgram = loadShaders("shaders/plain.vert", "shaders/plain.frag");
	glUseProgram(plainProgram);
	glUniform1i(glGetUniformLocation(plainProgram, "textureUnit"), texUnit);
	modelViewProjectionLocation =	glGetUniformLocation(plainProgram, "modelViewProjectionTransform");
	shadeLocation = glGetUniformLocation(plainProgram, "shade");

}


void drawPlain(Model* m, mat4 modelViewProjectionTransform, mat4 placement) {
	//	modelViewProjectionTransform = Mult(modelViewProjectionTransform, placement);
	glUseProgram(plainProgram);
	glUniform1f(shadeLocation, 0.3);
	glUniformMatrix4fv(modelViewProjectionLocation, 1, GL_TRUE, modelViewProjectionTransform.m);

	glBindVertexArray(m->vao);

	// Position
	glBindBuffer(GL_ARRAY_BUFFER, m->vb);
	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionLocation);

	glDrawElements(GL_TRIANGLES, m->numIndices, GL_UNSIGNED_INT, 0L);

}

void drawPlain2(Model *m, mat4 modelViewProjectionTransform) {
	glUseProgram(plainProgram);
	glUniform1f(shadeLocation, 0.3);

	//	glUniformMatrix4fv(modelViewProjectionLocation, 1, GL_TRUE, modelViewProjectionTransform.m);

	// Vertex positions.
	glBindVertexArray(m->vao);
	glBindBuffer(GL_ARRAY_BUFFER, m->vb);
	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionLocation);

	glDrawElements(GL_TRIANGLES, m->numIndices, GL_UNSIGNED_INT, 0L);
	printError("drawPlain()");
}
