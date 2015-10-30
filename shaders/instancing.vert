#version 150
#extension GL_ARB_explicit_attrib_location : enable

// "position" and "normal" are regular vertex attributes
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

// Color is a per-instance attribute
layout (location = 2) in vec2 textureCoordinates;
layout (location = 3) in vec4 test;

// instanceTrans will be used as a per-instance transformation
// matrix. Note that a mat4 consumes 4 consecutive locations, so
// this will actually sit in locations 4,  5, 6 and 7.
layout (location = 4) in mat4 instanceTrans;
// The view matrix and the projection matrix are constant
// across a draw
uniform mat4 projectionViewMatrix;

// The output of the vertex shader (matched to thead// fragment shader)
out VERTEX
{
	vec3 normal;
	vec2 textureCoordinates;
	vec4 test;
} vertex;

void main(void)
{
	// Transform position by the model-view matrix, then by thead
	// projection matrix.
	gl_Position = projectionViewMatrix * instanceTrans * position;
	// Transform the normal by the upper-left-3x3-submatrix of thead    // model-view matrix
	vertex.normal =  normal;
	// Pass the per-instance color through to the fragment shader.
	vertex.textureCoordinates = textureCoordinates;

	vertex.test = test;
}
