#pragma once
#include "maths_funcs.h"
#include "Model.h"
#include "Utils.h"

class Newspaper
{
public:
	vec3 Pos;
	vec3 Front;
	vec3 Up;
	vec3 Right;
	vec3 WorldUp = vec3(0.0f, 1.0f, 0.0f);
	float Yaw = 0.0f;
	float Pitch = 0.0f;
	float MovementSpeed;
	Model newspaper;
	GLfloat rotate_y = 0.0f;
	GLfloat rotate_y_sin = 00.0f;
	int remainingPapers = 10;
	float health;

	Newspaper();
	Newspaper(vec3 position);

	void renderNewspaper(mat4 gWVP, int shaderID);
	void ProcessKeyboard(MovementDir direction, float deltaTime);
	void MoveNewspaper(int x, int y, float deltaTime);
	void throwPapers(Model paper, Shader myShader, int remainingPapers);

	float convert(float degree) {
		float pi = M_PI;
		return (degree * (pi / 180));
	}

private:
	mat4 GetViewMatrix();
	void updateNewspaperVectors();
};