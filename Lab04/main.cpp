// Windows includes (For Time, IO, etc.)
/*
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl3.h"
*/
#define NOMINMAX
#include <limits>
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.
#include "theta.h"
#include "Shader.h"
#include "stb_image.h"
#include "Utils.h"
// OpenGL includes

#include <GL/glew.h>
#include <GL/freeglut.h>

//#include "Model.h"
// Project includes
//#include <glm/glm/glm.hpp>
//#include <glm/glm/gtc/matrix_transform.hpp>
//#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtx/euler_angles.hpp>
#include <glm/glm/gtc/quaternion.hpp>
#include <glm/glm/gtx/quaternion.hpp>
#include "maths_funcs.h"
#include "Fire.h"
#include "Camera.h"


Camera camera;
Camera isometricCamera;
int width = 1400;
int height = 700;
bool firstMouse = true;
GLfloat seal_forward = 0.0f;
GLfloat seal_right = 0.0f;
float delta;
float lastx = width / 2.0f;
float lasty = height / 2.0f;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
bool keyStates[256];
float skyR = 0.0f;
float skyG = 0.0f;
float skyB = 0.0f;
bool trans = false;
bool fog = false;
float spinning = 0.0f;
Model ground;
Model logs;
Model rocks;
using namespace std;

Shader phongShader;
Shader skyboxShader;
Shader fireShader;
Model terrain;
Model cube;
Fire fire;
mat4 gWVP;
Model teapot;

std::string parentDir = "D:\\Personal\\College\\5thYear\\Real Time Rendering\\Real Time Fire\\";

float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};
std::string facesCubemap[6] =
{
	parentDir + "Textures\\nx.png",
	parentDir + "Textures\\px.png",
	parentDir + "Textures\\nz.png",
	parentDir + "Textures\\py.png",
	parentDir + "Textures\\nz.png",
	parentDir + "Textures\\pz.png"
};
unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
unsigned int cubemapTexture;

mat4 resetRotation(mat4 matrix) {
	matrix.m[0] = 1;
	matrix.m[1] = 0;
	matrix.m[2] = 0;
	matrix.m[4] = 0;
	matrix.m[5] = 1;
	matrix.m[6] = 0;
	matrix.m[8] = 0;
	matrix.m[9] = 0;
	matrix.m[10] = 1;
	return matrix;
}

void display() {
	//Transparency
	if (trans == true) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else if (trans == false) {
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
	}
	glClearColor(skyR, skyG, skyB, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	phongShader.use();

	mat4 persp_proj = perspective(camera.Fov, (float)width / (float)height, 0.1f, 1000.0f);
	mat4 view = camera.GetViewMatrix();
	mat4 plane_mat = identity_mat4();

	phongShader.setFloat("density", 0.007);
	phongShader.setMat4("proj", persp_proj);
	phongShader.setMat4("view", view);
	phongShader.setVec3("viewPos", vec3(camera.Pos.v[0], camera.Pos.v[1], camera.Pos.v[2]));

	ground.RenderModel(plane_mat, phongShader.ID);
	rocks.RenderModel(plane_mat, phongShader.ID);
	logs.RenderModel(plane_mat, phongShader.ID);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE); //makes it look brighter
	fireShader.use();
	
	fire.firePlane.textureIndex++;
	fire.firePlane.textureIndex = fire.firePlane.textureIndex % 12;

	fireShader.setVec2("offset", vec2(fire.firePlane.getTextureXOffset(), fire.firePlane.getTextureYOffset()));

	mat4 billboardView = camera.GetViewMatrix();
	billboardView = resetRotation(billboardView);

	fireShader.setFloat("numberOfRows", 3.0f);
	fireShader.setMat4("view", billboardView);
	fireShader.setMat4("proj", persp_proj);
	fireShader.setVec3("view", vec3(camera.Pos.v[0], camera.Pos.v[1], camera.Pos.v[2]));
	plane_mat = scale(plane_mat, vec3(2.0f, 2.0f, 2.0f));
	fire.renderFire(plane_mat, fireShader.ID);
	
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	
	if (fog == true) {
		phongShader.setFloat("density", 0.007);
	}
	else {
		phongShader.setFloat("density", 0.000);
	}
	glutSwapBuffers();
}

void updateCamera() {
	
	if (camera.freeCam == true) {
		if (keyStates['w'] == true) { //move cam forward
			camera.ProcessKeyboard(FORWARD, delta);
		}
		if (keyStates['a'] == true) {//mpve cam left
			camera.ProcessKeyboard(LEFT, delta);
		}
		if (keyStates['d'] == true) {//move cam right
			camera.ProcessKeyboard(RIGHT, delta);
		}
		if (keyStates['s'] == true) {//move cam backward
			camera.ProcessKeyboard(BACKWARD, delta);
		}
		if (keyStates['q'] == true) {//move cam backward
			camera.ProcessKeyboard(DOWN, delta);
		}
		if (keyStates['e'] == true) {//move cam backward
			camera.ProcessKeyboard(UP, delta);
		}


	}
}

void updateScene() {
	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;
	fire.updateBillboard(camera);
	updateCamera();
	glutPostRedisplay();
}

void init()
{
	// Set up the models and shaders
	phongShader = Shader("D:\\Personal\\College\\5thYear\\Real Time Rendering\\Real Time Fire\\simpleVertexShader.txt","D:\\Personal\\College\\5thYear\\Real Time Rendering\\Real Time Fire\\simpleFragmentShader.txt");
	fire = Fire(vec3(0.0f, 3.0f, 0.0f));
	fireShader = Shader("D:\\Personal\\College\\5thYear\\Real Time Rendering\\Real Time Fire\\Lab04\\textureAtlasVertexShader.txt", "D:\\Personal\\College\\5thYear\\Real Time Rendering\\Real Time Fire\\simpleFragmentShader.txt");
	skyboxShader = Shader("D:\\Personal\\College\\5thYear\\Real Time Rendering\\Real Time Fire\\Lab04\\skyboxVert.txt", "D:\\Personal\\College\\5thYear\\Real Time Rendering\\Real Time Fire\\Lab04\\skyboxFrag.txt");
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);
	ground = Model("D:\\Personal\\College\\5thYear\\Real Time Rendering\\Real Time Fire\\Models\\Ground.obj");
	logs = Model("D:\\Personal\\College\\5thYear\\Real Time Rendering\\Real Time Fire\\Models\\Logs.obj");
	rocks = Model("D:\\Personal\\College\\5thYear\\Real Time Rendering\\Real Time Fire\\Models\\Rocks.obj");
}

void mouseCallback(int x, int y) {

	if (firstMouse)
	{
		lastx = x;
		lasty = y;
		firstMouse = false;
	}

	static bool wrap = false;
	float dx = 0.0;
	float dy = 0.0;
	
	if (!wrap) {
		int ww = glutGet(GLUT_WINDOW_WIDTH);
		int wh = glutGet(GLUT_WINDOW_HEIGHT);

		dx = x - ww / 2;
		dy = y - wh / 2;

		// Do something with dx and dy here

		// move mouse pointer back to the center of the window
		wrap = true;
		glutWarpPointer(ww / 2, wh / 2);
	}
	else {
		wrap = false;
	}

	float sensitivity = 0.1f;
	dx *= sensitivity;
	dy *= sensitivity;

	yaw += dx;
	pitch -= dy;

	if (pitch > 89.0f)
		pitch = 89.0f;

	if (pitch < -89.0f)
		pitch = -89.0f;
	if (camera.thirdPerson == false) {
		camera.ProcessMouseMovement(dx, -dy, true);
	}
}

void keyDown(unsigned char key, int x, int y) {
	keyStates[key] = true;
	if (key == 'z') {
		cout << "z key pressed";
		camera.thirdPerson = true;
		camera.originalCamera = false;
		camera.freeCam = false;
	}
	if (key == 'x') {
		camera.thirdPerson = false;
		camera.originalCamera = true;
		camera.freeCam = false;
		fog = false;
	}
	if (key == 'c') {
		camera.thirdPerson = false;
		camera.originalCamera = false;
		camera.freeCam = true;
		fog = false;
	}
}

void keyUp(unsigned char key, int x, int y) {
	keyStates[key] = false;
}

int main(int argc, char** argv) {
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO(); (void)io;
	//ImGui::StyleColorsDark();
	//ImGui_ImplGLUT_Init();


	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Real Time Animation");

	// Tell glut where the display function is

	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutPassiveMotionFunc(mouseCallback);
	//glutMouseFunc(mouseCallback);
	glutSetCursor(GLUT_CURSOR_NONE);
	

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();


	//}
	// Create VAO, VBO, and EBO for the skybox
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Creates the cubemap texture object
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// This might help with seams on some systems
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}