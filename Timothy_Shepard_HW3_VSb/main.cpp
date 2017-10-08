// 
//  Timothy Shepard
//  HW2
//  Rectangle with Shaders
//
#include <Windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

const int NUM_VERTICES = 4;
const int NUM_INDICES = 2687;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
GLuint shaderProgramID;
GLuint vao = 0;
GLuint vbo;
GLuint positionID, colorID;
GLuint indexBufferID;

struct Vertex {
	GLfloat x, y, z;
	GLfloat r, g, b, a;
};

struct ShapeData {
	Vertex* vertices;
	GLuint numVertices;
	GLushort* indices;
	GLuint numIndices;
};



///////////////////////////////////////////////////////////////
// Read and Compile Shaders from tutorial
///////////////////////////////////////////////////////////////
static char* readFile(const char* filename) {
	FILE* fp = fopen(filename, "r");
	fseek(fp, 0, SEEK_END);
	long file_length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* contents = new char[file_length + 1];
	for (int i = 0; i < file_length + 1; i++) {
		contents[i] = 0;
	}
	fread(contents, 1, file_length, fp);
	contents[file_length + 1] = '\0';  // end of string in C
	fclose(fp);
	return contents;
}

bool compiledStatus(GLint shaderID) {
	GLint compiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (compiled) {
		return true;
	}
	else {
		GLint logLength;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
		char* msgBuffer = new char[logLength];
		glGetShaderInfoLog(shaderID, logLength, NULL, msgBuffer);
		printf("%s\n", msgBuffer);
		delete (msgBuffer);
		return false;
	}
}

void printShaderSource(const char* shaderSource, string shaderName) {
	printf("--------------------------------------------------------------\n");
	printf("Printing %s shader:\n", shaderName.c_str());
	printf(shaderSource);
	printf("\n");
}

GLuint makeVertexShader(const char* shaderSource) {
	printShaderSource(shaderSource, "vertex");
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(vertexShaderID);
	bool compiledCorrectly = compiledStatus(vertexShaderID);
	if (compiledCorrectly) {
		return vertexShaderID;
	}
	return -1;
}

GLuint makeFragmentShader(const char* shaderSource) {
	printShaderSource(shaderSource, "fragment");
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(fragmentShaderID);
	bool compiledCorrectly = compiledStatus(fragmentShaderID);
	if (compiledCorrectly) {
		return fragmentShaderID;
	}
	return -1;
}

GLuint makeShaderProgram(GLuint vertexShaderID, GLuint fragmentShaderID) {
	GLuint shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShaderID);
	glAttachShader(shaderID, fragmentShaderID);
	glLinkProgram(shaderID);
	return shaderID;
}

///////////////////////////////////////////////////////////////
// Read File for Tris.txt
///////////////////////////////////////////////////////////////
int getNumTriangles(string filename) 
{
	ifstream inputFile;
	inputFile.open(filename);
	int numTriangles = 0;
	inputFile >> numTriangles;
	inputFile.close();
	return numTriangles;
}

Vertex* getAllVertices(string filename) 
{
	ifstream inputFile;
	inputFile.open(filename);

	int numTriangles = 0;
	inputFile >> numTriangles;

	Vertex* allVertices;
	allVertices = new Vertex[numTriangles*3];  // numVertices is numTriangles*3

	int currentIndex = 0;
	while (!inputFile.eof())
	{
		string currentLine;
		getline(inputFile, currentLine);
		if (currentLine == "") { continue;}
		stringstream ss;
		ss << currentLine;
		Vertex newVertex;
		ss >> newVertex.x >> newVertex.y >> newVertex.z;
		ss >> newVertex.r >> newVertex.g >> newVertex.b >> newVertex.a;
		allVertices[currentIndex] = newVertex;
		currentIndex++;
	}

	inputFile.close();
	return allVertices;
}

void printAllVertices(Vertex* allVertices, int numTriangles)
{
	int triangleCount = 0;
	cout << "Printing all vertices.\n";
	for (int i = 0; i < numTriangles * 3; i++)
	{
		if (i % 3 == 0) {cout << "Triangle " << ++triangleCount << endl;}
		Vertex v;
		v = allVertices[i];
		cout << "Vertex: " << i << " ";
		cout << v.x << " " << v.y << " " << v.z << " ";
		cout << v.r << " " << v.b << " " << v.g << " " << v.a << endl;
	}
}

glm::vec3* buildPositionsVec3s(Vertex* allVertices, int numVertices)
{
	glm::vec3* allPositions;
	allPositions = new glm::vec3[numVertices];
	for (int i = 0; i < numVertices; i++)
	{
		Vertex v;
		v = allVertices[i];
		allPositions[i] = glm::vec3(v.x, v.y, v.z);
	}
	return allPositions;
}

void printAllPositions(glm::vec3* allPositions, int numVertices)
{
	cout << "Printing all position vec3:\n";
	for (int i = 0; i < numVertices; i++)
	{
		cout << "Position For Vertex: " << i << " ";
		cout << allPositions[i][0] << " " << allPositions[i][1];
		cout << " " << allPositions[i][2] << endl;
	}
}

void printAllColors(glm::vec4* allColors, int numVertices)
{
	cout << "Printing all color vec4:\n";
	for (int i = 0; i < numVertices; i++)
	{
		cout << "Color For Vertex: " << i << " ";
		cout << allColors[i][0] << " " << allColors[i][1] << " ";
		cout << allColors[i][2] << " " << allColors[i][3] << endl;
	}
}

glm::vec4* buildColorsVec4s(Vertex* allVertices, int numVertices)
{
	glm::vec4* allColors;
	allColors = new glm::vec4[numVertices];
	for (int i = 0; i < numVertices; i++)
	{
		Vertex v;
		v = allVertices[i];
		allColors[i] = glm::vec4(v.r, v.g, v.b, v.a);
	}
	return allColors;
}

GLuint* getTriangleIndicesArray(int numVertices) 
{
	GLuint* triangleIndicesList;
	triangleIndicesList = new GLuint[numVertices];
	for (int i = 0; i < numVertices; i++)
	{
		triangleIndicesList[i] = GLuint(i);
	}
	return triangleIndicesList;
}

void printTriangleIndices(GLuint* triangleIndices, int numVertices)
{
	cout << "Printing all indices.\n";
	for (int i = 0; i < numVertices; i++)
	{
		cout << triangleIndices[i] << "\n";
	}
}

///////////////////////////////////////////////////////////////
// Main Part of program
///////////////////////////////////////////////////////////////

// changeViewport
void changeViewport(int w, int h) {
	glViewport(0, 0, w, h);
}

// render
void render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, NULL);
	glutSwapBuffers();
}

int main(int argc, char** argv) {

	// Get vertices from Tris.txt file
	string filename = "Tris.txt";
	int numTriangles = getNumTriangles(filename);
	Vertex* allVertices;
	allVertices = getAllVertices(filename);
	int numVertices = numTriangles * 3;
	//printAllVertices(allVertices, numTriangles);
	glm::vec3* vpositions;
	vpositions = buildPositionsVec3s(allVertices, numVertices);
	glm::vec4* vcolors;
	vcolors = buildColorsVec4s(allVertices, numVertices);

	int numIndices;
	numIndices = numVertices;
	GLuint* triangleIndices;
	triangleIndices = getTriangleIndicesArray(numVertices);
	//printTriangleIndices(triangleIndices, numVertices);
	//printAllPositions(vpositions, numVertices);
	//printAllColors(vcolors, numVertices);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Timothy_Shepard_HW3.zip");
	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glewInit();  //glewInit() for Windows only
	cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;
	cout << "GL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n\n";

	GLfloat vertices[] = { -0.5f,  0.5f, 0.0f, // 0
		-0.5f, -0.5f, 0.0f, // 1
		0.5f, -0.5f, 0.0f, // 2
		0.5f,  0.5f, 0.0f  // 3
	};
	GLfloat colors[] = { 1.0f, 1.00f, 1.0f, 1.0f, // 0
		1.0f, 0.00f, 0.0f, 1.0f, // 1
		0.0f, 1.00f, 0.0f, 1.0f, // 2
		0.0f, 0.00f, 1.0f, 1.0f  // 3
	};

	GLuint indices[] = { 0, 1, 3, 1, 2, 3 };


	

	// Make a shader
	char* vertexShaderSourceCode = readFile("vertexShader.vsh");
	char* fragmentShaderSourceCode = readFile("fragmentShader.fsh");
	GLuint vertShaderID = makeVertexShader(vertexShaderSourceCode);
	GLuint fragShaderID = makeFragmentShader(fragmentShaderSourceCode);
	shaderProgramID = makeShaderProgram(vertShaderID, fragShaderID);

	// Change for APPLE version
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//glGenVertexArraysAPPLE(1, &vao);
	//glBindVertexArrayAPPLE(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 7 * numVertices * sizeof(GLfloat), NULL, GL_STATIC_DRAW); //Create buffer
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * numVertices * sizeof(GLfloat), vpositions);  // Put data in buffer
	glBufferSubData(GL_ARRAY_BUFFER, 3 * numVertices * sizeof(GLfloat), 4 * numVertices * sizeof(GLfloat), vcolors);

	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), triangleIndices, GL_STATIC_DRAW); // Put indices in buffer as Gluint

																								  // Find the position of the variables in the shader
    positionID = glGetAttribLocation(shaderProgramID, "s_vPosition");
	colorID = glGetAttribLocation(shaderProgramID, "s_vColor");

	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vpositions)));
	glUseProgram(shaderProgramID);
	glEnableVertexAttribArray(positionID);
	glEnableVertexAttribArray(colorID);

	glutMainLoop();

	delete[] allVertices;
	delete[] triangleIndices;
	delete[] vpositions;
	delete[] vcolors;

	return 0;
}
