#include <vector>
#include <iostream>
#include <cstdlib>
#include "Airplane.h"
#include "Newspaper.h"
#include "Theta.h"

Airplane::Airplane(vec3 position)
{
	airplane = Model("D:\\Personal\\College\\5thYear\\Real Time Animation\\Models\\Bones.obj");
	Pos = position;
	MovementSpeed = 5.0f;
	Front = vec3(1.0f, 0.0f, 0.0f);
	Right = vec3(0.0f, 1.0f, 0.0f);
	mats.push_back(identity_mat4());
	mats.push_back(identity_mat4());
	mats.push_back(identity_mat4());
	mats.push_back(identity_mat4());
	mats[2] = translate(mats[1], vec3(0.0f, 4.72952f, 0.0f));
	mats[3] = translate(mats[3], vec3(0.0f, 8.72952f, 0.0f));
}

Airplane::Airplane()
{

}

void Airplane::renderAirplane(mat4 pos, int shaderID) {

	//Transform the PaperBoy
		//pos = translate(pos, vec3(-pos.m[0], -pos.m[5], -pos.m[10]));

	mat4 airplane_pos = identity_mat4();

	int world_loc = glGetUniformLocation(shaderID, "world");
	glUniformMatrix4fv(world_loc, 1, GL_FALSE, airplane_pos.m);

	airplane.RenderMesh(mats[0], shaderID, 0);
	airplane.RenderMesh(mats[1], shaderID, 1);
	airplane.RenderMesh(mats[2], shaderID, 2);
	airplane.RenderMesh(mats[3], shaderID, 3);
}

mat4 mat4ToPrevBone(std::vector<mat4> mats, mat4 mat1, int i) {
	//int index = getIndex(mats, mat1);
	mat4 prevMat = mats[i-1];
	mat1.m[12] = prevMat.m[12];
	mat1.m[13] = prevMat.m[13];
	mat1.m[14] = prevMat.m[14];
	return mat1;
}
mat4 flipSignOfPos(mat4 mat, mat4 pos) {
	mat.m[12] = -pos.m[12];
	mat.m[13] = -pos.m[13];
	mat.m[14] = -pos.m[14];
	return mat;
}

void Airplane::RotateZ(int nodeNum, bool pos) {
	mat4 moveMat = identity_mat4();
	int sign = 1;
	if (pos == false) sign = -1;
	else { sign = 1; }
	moveMat = flipSignOfPos(moveMat, mats[nodeNum]);

	for (int i = nodeNum; i < mats.size(); i++) {
		mats[i] = moveMat * mats[i];
	}
	for (int i = nodeNum; i < mats.size(); i++) {
		mats[i] = rotate_z_deg(mats[i], 10.0f * sign);
	}
	moveMat = flipSignOfPos(moveMat, moveMat);
	for (int i = nodeNum; i < mats.size(); i++) {
		mats[i] = moveMat * mats[i];
	}
}

void Airplane::ProcessKeyboard(MovementDir direction, float deltaTime)
{
	//std::cout << "\n x" << mat2.m[12] << " y " << mat2.m[13] << " z " << mat2.m[14];
	if (direction == LEFT) {
		RotateZ(1, true);
	}
	if (direction == RIGHT) {
		RotateZ(1, false);
	}
	if (direction == FORWARD) {
		RotateZ(2, true);
	}
	if (direction == BACKWARD) {
		RotateZ(2, false);
	}
	if (direction == UP) {
		RotateZ(3, false);
	}
	if (direction == ROLL) {
		RotateZ(3, true);
	}
	if (direction == MOVE_LEFT) {
		mat4 moveMat = identity_mat4();
		float speed = -0.1;
		moveMat.m[12] = speed;
		mats[0] = moveMat * mats[0];
		mats[1] = moveMat * mats[1];
		mats[2] = moveMat * mats[2];
		mats[3] = moveMat * mats[3];
	}
	if (direction == MOVE_RIGHT) {
		mat4 moveMat = identity_mat4();
		float speed = 0.1;
		moveMat.m[12] = speed;
		mats[0] = moveMat * mats[0];
		mats[1] = moveMat * mats[1];
		mats[2] = moveMat * mats[2];
		mats[3] = moveMat * mats[3];
	}
}

float Airplane::CalcDist(Model cube) {
	float pow1 = pow(cube.location.m[12] - mats[3].m[12], 2);
	float pow2 = pow(cube.location.m[13] - mats[3].m[13], 2);
	float temp = pow1 + pow2;
	float dist = sqrt(temp);

	std::cout << "\n Distance to Block" << dist;
	return dist;
}
/*
void Airplane::Fabrik(Model cube) {
	vec3 cubePos = vec3(cube.location.m[12], cube.location.m[13], cube.location.m[14]);
	vec3 endPos = vec3(mats[3].m[12], mats[3].m[13], mats[13].m[13]);
	//float dist = abs(endPos - cubePos);
	while (CalcDist(cube) > EPS) {
		//FinalToRoot(); // PartOne
		//RootToFinal(); // PartTwo
	}
}

void Airplane::FinalToRoot() {
	currentGoal = goalPosition;
	currentLimb = mats[3];
	while (&currentLimb != NULL) {
		currentLimb.rotation = RotFromTo(Vector.UP, currentGoal - currentLimb.inboardPosition);
		currentLimb.outboardPosition = currentGoal;
		currentGoal = currentLimb.inboardPosition;
		currentLimb = currentLimb->inboardLimb;
	}
}

void Airplane::RootToFinal() {
	currentInboardPosition = rootLimb.inboardPosition;
	currentLimb = rootLimb;
	while (currentLimb != NULL) {
		currentLimb.inboardPosition = currentInboardPosition;
		currentInboardPosition = currentLimb.outboardPosition;
		currentLimb = currentLimb->outboardLimb;
	}
}
*/
void Airplane::UpdateTheta(int x, int y)
{

	std::cout << "\n Screen x and y :" << x << " " << y << " ";
	x_pos = x;
	y_pos = y;
	Angle obj1;
	theta = obj1.calcTheta(vec2(x_pos, y_pos));
	alpha = 160;
	std::cout << " theta: " << theta.v[0] << " " << theta.v[1];
	if (isnan(theta.v[0]) && isnan(theta.v[1]))
	{
		std::cout << " nan";
		theta.v[0] = 0;
		theta.v[1] = 0;
		alpha = 0;
	}
}

mat4 Airplane::GetViewMatrix()
{
	return look_at(Pos, Pos + Front, Up);
}

void Airplane::updateAirplaneVectors()
{
	vec3 front;
	front.v[0] = cos(convert(-Yaw)) * cos(convert(Pitch));
	front.v[1] = sin(convert(Pitch));
	front.v[2] = sin(convert(-Yaw)) * cos(convert(Pitch));
	Front = normalise(front);
	Right = normalise(cross(Front, WorldUp));
	Up.v[1] = Up.v[1] * cos(Roll);
	Up = normalise(cross(Right, Front));
}

void Airplane::spinWheels(float delta) {
	rotate_y_sin += 500.0f * delta;
}

