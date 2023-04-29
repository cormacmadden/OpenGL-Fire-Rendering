#pragma once
#include <vector>
#include <cstdlib>
#include "maths_funcs.h"
#include "Model.h"
#include "Utils.h"
#include "Newspaper.h"
#include "Camera.h"
#include <list>


class Fire
{
public:
	vec3 Pos;
	vec3 Front;
	vec3 Up;
	vec3 Right;
	vec3 WorldUp = vec3(0.0f, 1.0f, 0.0f);
	float Yaw = 00.0f;
	float Pitch = 0.0f;
	float Roll = 0.0f;
	float MovementSpeed;
	Model firePlane;
	GLfloat rotate_y = 0.0f;
	GLfloat rotate_y_sin = 00.0f;
	int remainingPapers = 10;
	float health;
	std::vector<mat4> mats;
	float EPS = 0.02;
	//mat4 mat0 = identity_mat4();
	//mat4 mat1 = identity_mat4();
	//mat4 mat2 = identity_mat4();
	//mat4 mat3 = identity_mat4();
	float x_pos, y_pos, angle;
	Fire();

	Fire(vec3 position);
	//angles for rotations
	GLfloat rotate_x_angle = 0.0f;
	GLfloat rotate_y_angle = 0.0f;
	GLfloat rotate_z_angle = 0.0f;
	vec2 theta;
	float alpha = 0;
	void Fabrik(Model cube);

	void renderFire(mat4 gWVP, int shaderID);
	void ProcessKeyboard(MovementDir direction, float deltaTime);
	void throwPapers(Newspaper paper, Shader myShader, int remainingPapers);

	void spinWheels(float delta);

	float convert(float degree) {
		float pi = M_PI;
		return (degree * (pi / 180));
	}

	void UpdateTheta(int x, int y);
	
	mat4 currentLimb;
	mat4 currentGoal;
	mat4 currentInboardPosition;
	mat4 goalPosition;
	mat4 finalLimb;
	void updateBillboard(Camera camera);
private:
	void RotateZ(int nodeNum, bool pos);
	void RotateY(int nodeNum, bool pos);
	float CalcDist(Model cube);
	mat4 GetViewMatrix();
	void updateAirplaneVectors();
	
	//void FinalToRoot();
	//void RootToFinal();
};