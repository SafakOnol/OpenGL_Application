#include <glew.h>
#include <glut.h>
#include <freeglut.h>
#include <stdio.h>

static void RenderSceneCB()
{
	static GLclampf c = 0.0f;
	glClearColor(c, c, c, c);
	printf("%f\n", c);

	c += 1.0f / 256.0f;
	if (c >= 1.0f) c = 0.0f;

	glClear(GL_COLOR_BUFFER_BIT); // get rid of screen clutter if any
	glutPostRedisplay(); // forces screen to keep rendering
	glutSwapBuffers(); // eliminate screen tearing
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	int width = 800;
	int height = 600;
	glutInitWindowSize(width, height);

	int x = 200;
	int y = 100;
	glutInitWindowPosition(x, y);
	int win = glutCreateWindow("Tutorial 01");
	printf("window id: %d\n", win);

	GLclampf Red = 0.0f, Green = 0.0f, Blue = 0.0f, Alpha = 0.0f;
	glClearColor(Red, Green, Blue, Alpha);

	glutDisplayFunc(RenderSceneCB);

	glutMainLoop();

	return 0;
}