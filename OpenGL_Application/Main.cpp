#include <stdio.h>
#include <string.h>

#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogldev_math_3d.h"

GLuint VBO;
//GLint gScaleLocation;
GLint gTranslationLocation;

static void RenderSceneCB()
{
	glClear(GL_COLOR_BUFFER_BIT);

	// set the scale function
	static float Scale = 0.0f;
	static float Delta = 0.001f;

	Scale += Delta;
	if ((Scale >= 1.0f) || (Scale <= -1.0f)) 
	{
		Delta *= -1.0f;
	}

	// Construct Translation Matrix
	Matrix4f Translation(1.0f, 0.0f, 0.0f, Scale * 2.0f,
						 0.0f, 1.0f, 0.0f, Scale,
						 0.0f, 0.0f, 1.0f, 0.0f,
						 0.0f, 0.0f, 0.0f, 1.0f);

	// Load the matrix into the shader
	// 1. The index that syncs c++ matrix with the matrix in the shader
	// 2. Number of matrices to send down to shader
	// 3. Decide if matrix is row-major(true) or column-major(false)
	// 4. The address of the array
	glUniformMatrix4fv(gTranslationLocation, 1, GL_TRUE, &Translation.m[0][0]);

	// send the scale value down to shader
	//glUniform1f(gScaleLocation, Scale);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 3); // Create triangles from every 3 consecutive coordinates in the bound vertex buffer object

	glDisableVertexAttribArray(0);

	glutPostRedisplay();

	glutSwapBuffers();
}

static void CreateVertexBuffer()
{
	//glEnable(GL_CULL_FACE); // enable face culling
	//glFrontFace(GL_CW); // tell OpenGL that ClockWise is front facing
	//glCullFace(GL_FRONT); // cull front facing triangles by default

	//define triangle vertices
	Vector3f Vertices[3];
	Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f); // bottom left	
	Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f); //bottom right
	Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f); // top
	

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// 1: Create the shader handle
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}

	// The array of string pointers
	const GLchar* p[1];
	p[0] = pShaderText;

	// The array of length
	GLint Lengths[1];
	Lengths[0] = (GLint)strlen(pShaderText);

	// 2: Load shader text into the shader handle
	// p1: Shader handle
	// p2: Number of elements in each of the arrays (p3 & p4)
	// p3: Pointers to an array of string
	// p4: Pointers to an array of lengths    
	glShaderSource(ShaderObj, 1, p, Lengths);

	glCompileShader(ShaderObj);

	// 3: Check compilation errors
	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}

	// 4: If there's no compilation errors, attach the shader to the handle
	// p1: Program handle
	// p2: Shader handle
	glAttachShader(ShaderProgram, ShaderObj);
}

const char* pVSFileName = "shaderVert.glsl";
const char* pFSFileName = "shader Frag.glsl";

static void CompileShaders()
{
	// 5: Load the source for the shaders, use AddShader() to compile and attach them
	GLuint ShaderProgram = glCreateProgram();

	if (ShaderProgram == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	std::string vs, fs;

	if (!ReadFile(pVSFileName, vs)) {
		exit(1);
	};

	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	if (!ReadFile(pFSFileName, fs)) {
		exit(1);
	};

	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	// 6: Link the program with program handle
	glLinkProgram(ShaderProgram);

	// check compilation errors
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	// allocate the uniform location index
	// this should be done after linking the program with glLinkProgram
	/*gScaleLocation = glGetUniformLocation(ShaderProgram, "gScale");
	if (gScaleLocation == -1) {
		printf("Error getting uniform location of 'gScale'\n");
		exit(1);
	}*/

	gTranslationLocation = glGetUniformLocation(ShaderProgram, "gTranslation");
	if (gTranslationLocation == -1) 
	{
		printf("Error getting uniform location of 'gTranslation'\n");
		exit(1);
	}

	// check if linked program is compatible with the current state of OpenGL runtime
	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	// 7: Activate the program and the shaders on the GPU
	glUseProgram(ShaderProgram);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	int width = 800;
	int height = 600;
	glutInitWindowSize(width, height);

	int x = 200;
	int y = 100;
	glutInitWindowPosition(x, y);
	int win = glutCreateWindow("Tutorial 03");
	printf("window id: %d\n", win);

	// Must be done after glut is initialized!
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	GLclampf Red = 0.0f, Green = 0.0f, Blue = 0.0f, Alpha = 0.0f;
	glClearColor(Red, Green, Blue, Alpha);

	CreateVertexBuffer();

	CompileShaders();

	glutDisplayFunc(RenderSceneCB);

	glutMainLoop();

	return 0;
}