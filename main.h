/**
 * Draws the object with the specified transform.
 *
 * @param transform The transform to be applied to the model.
 * @param model The model to be drawn.
 */
void drawObject(mat4 transform, Model* model, GLuint p);

void handleMouse(int x, int y);

GLuint fullProgram, plainProgram, instancingProgram;
