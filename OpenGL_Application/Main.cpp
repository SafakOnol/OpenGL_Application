#include <stdio.h>
#include <string.h>

#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogldev_math_3d.h"

#define WINDOW_WIDTH  960
#define WINDOW_HEIGHT 540

GLuint VBO;
GLuint IBO;
GLuint gWVPLocation;

static void RenderSceneCB()
{
	glClear(GL_COLOR_BUFFER_BIT);

	static float Scale = 0.0f;

#ifdef _WIN64
	Scale += 0.002f;
#else
	Scale += 0.02f;
#endif

	Matrix4f Rotation(	cosf(Scale),	0.0f,	-sinf(Scale),	0.0f,
						0.0f,			1.0f,	0.0f,			0.0f,
						sinf(Scale),	0.0f,	cosf(Scale),	0.0f,
						0.0f,			0.0f,	0.0f,			1.0f);

	Matrix4f Translation(	1.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 1.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 2.0f,
							0.0f, 0.0f, 0.0f, 1.0f);

	// World transformation
	Matrix4f World = Translation * Rotation;

	Vector3f CameraPos(0.0f, 0.0f, 0.0f);
	Vector3f U(1.0f, 0.0f, 0.0f);
	Vector3f V(0.0f, 1.0f, 0.0f);
	Vector3f N(0.0f, 0.0f, 1.0f);

	Matrix4f Camera(U.x,	U.y,	U.z, -CameraPos.x,
					V.x,	V.y,	V.z, -CameraPos.y,
					N.x,	N.y,	N.z, -CameraPos.z,
					0.0f,	0.0f,	0.0f, 1.0f);


	float FOV = 90.0f;
	float tanHalfFOV = tanf(ToRadian(FOV / 2.0f));
	float d = 1 / tanHalfFOV;
	float ar = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

	//printf("Aspect ratio %f\n", ar);

	float NearZ = 1.0f;
	float FarZ = 100.0f;

	float zRange = NearZ - FarZ;

	float A = (-FarZ - NearZ) / zRange;
	float B = 2.0f * FarZ * NearZ / zRange;


	Matrix4f Projection(d/ar, 0.0f, 0.0f, 0.0f,
						0.0f, d,    0.0f, 0.0f,
						0.0f, 0.0f, A,    B,
						0.0f, 0.0f, 1.0f, 0.0f);

	Matrix4f WVP = Projection * Camera * World;

	glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, &WVP.m[0][0]);

	// Bind the buffers respectively
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// position
	glEnableVertexAttribArray(0);
	// index - size - type - normalized boolean - stride size - pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

	// color
	glEnableVertexAttribArray(1);
	// color starts at the 4th position (number 3) in the vertex, therefore we set the pointer 3 * size of a float and cast it to a void pointer
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glutPostRedisplay();

	glutSwapBuffers();
}

struct Vertex
{
	Vector3f pos;
	// define the color in the Vertex Buffer rather than in the shader
	Vector3f color;

	// default constructor
	Vertex() {}

	Vertex(float x, float y, float z)
	{
		pos = Vector3f(x, y, z);
		// get a random value between 0 and 1
		float red = (float)rand() / (float)RAND_MAX;
		float green = (float)rand() / (float)RAND_MAX;
		float blue = (float)rand() / (float)RAND_MAX;
		color = Vector3f(red, green, blue);
	}
};


static void CreateVertexBuffer()
{
	Vertex Vertices[8];

	Vertices[0] = Vertex(0.5f, 0.5f, 0.5f);
	Vertices[1] = Vertex(-0.5f, 0.5f, -0.5f);
	Vertices[2] = Vertex(-0.5f, 0.5f, 0.5f);
	Vertices[3] = Vertex(0.5f, -0.5f, -0.5f);
	Vertices[4] = Vertex(-0.5f, -0.5f, -0.5f);
	Vertices[5] = Vertex(0.5f, 0.5f, -0.5f);
	Vertices[6] = Vertex(0.5f, -0.5f, 0.5f);
	Vertices[7] = Vertex(-0.5f, -0.5f, 0.5f);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void CreateIndexBuffer()
{
	unsigned int Indices[] = {
							 0, 1, 2,
							 1, 3, 4,
							 5, 6, 3,
							 7, 3, 6,
							 2, 4, 7,
							 0, 7, 6,
							 0, 5, 1,
							 1, 5, 3,
							 5, 0, 6,
							 7, 4, 3,
							 2, 1, 4,
							 0, 2, 7
	};

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// 1: Create the shader handle
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(1);
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
const char* pFSFileName = "shaderFrag.glsl";

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

	/*gTranslationLocation = glGetUniformLocation(ShaderProgram, "gTranslation");
	if (gTranslationLocation == -1) 
	{
		printf("Error getting uniform location of 'gTranslation'\n");
		exit(1);
	}*/

	/*gRotationLocation = glGetUniformLocation(ShaderProgram, "gRotation");
	if (gRotationLocation == -1)
	{
		printf("Error getting uniform location of 'gRotation'\n");
		exit(1);
	}*/

	/*gScalingLocation = glGetUniformLocation(ShaderProgram, "gScaling");
	if (gScalingLocation == -1)
	{
		printf("Error getting uniform location of 'gScaling'\n");
		exit(1);
	}*/

	gWVPLocation = glGetUniformLocation(ShaderProgram, "gWVPLocation");
	if (gWVPLocation == -1) {
		printf("Error getting uniform location of 'gWVPLocation'\n");
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
	// this is to get random colors
#ifdef _WIN64
	srand(GetCurrentProcessId());
#else
	srandom(getpid());
#endif
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	int x = 400;
	int y = 0;
	glutInitWindowPosition(x, y);
	int win = glutCreateWindow("Tutorial 12");
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

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	CreateVertexBuffer();
	CreateIndexBuffer();

	CompileShaders();

	glutDisplayFunc(RenderSceneCB);

	glutMainLoop();

	return 0;
}