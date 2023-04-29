#include <vector>
#include <iostream>
#include <cstdlib>
#include "Fire.h"
#include "Newspaper.h"
#include "Theta.h"


Fire::Fire(vec3 position)
{
	firePlane = Model("D:\\Personal\\College\\5thYear\\Real Time Rendering\\Real Time Fire\\Models\\FirePlane.obj");
	Pos = position;
	MovementSpeed = 5.0f;
	Front = vec3(1.0f, 0.0f, 0.0f);
	Right = vec3(0.0f, 1.0f, 0.0f);
	mat4 newPos = translate(identity_mat4(), Pos);
	mats.push_back(newPos);
}

Fire::Fire()
{

}

void Fire::renderFire(mat4 pos, int shaderID) {

	//Transform the PaperBoy
		//pos = translate(pos, vec3(-pos.m[0], -pos.m[5], -pos.m[10]));

	mat4 firePlane_pos = identity_mat4();
	mat4 pos1 = translate(pos, vec3(0.0f, 2.0f, 0.0f));
	int world_loc = glGetUniformLocation(shaderID, "world");
	glUniformMatrix4fv(world_loc, 1, GL_FALSE, pos1.m);

	firePlane.RenderMesh(pos1, shaderID, 0);
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

void Fire::RotateZ(int nodeNum, bool pos) {
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

void Fire::ProcessKeyboard(MovementDir direction, float deltaTime)
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

float Fire::CalcDist(Model cube) {
	float pow1 = pow(cube.location.m[12] - mats[3].m[12], 2);
	float pow2 = pow(cube.location.m[13] - mats[3].m[13], 2);
	float temp = pow1 + pow2;
	float dist = sqrt(temp);

	std::cout << "\n Distance to Block" << dist;
	return dist;
}

void Fire::UpdateTheta(int x, int y)
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

mat4 Fire::GetViewMatrix()
{
	return look_at(Pos, Pos + Front, Up);
}

void Fire::updateAirplaneVectors()
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

void Fire::updateBillboard(Camera camera)
{
	//mat4 bill_mat = look_at(camera.Pos, Pos, WorldUp);
	mat4 view = camera.GetViewMatrix();
	mat4 inv_view = inverse(view);
	mat4 test = mats[0] * inv_view;
	mat4 test2 = test * view; 
	/*
	vec3 CameraRight_worldspace = vec3(view.m[0], view.m[4], view.m[8]);
	vec3 CameraUp_worldspace = vec3(view.m[1], view.m[5], view.m[9]);
	vec3 vertexPosition_worldspace =
		Pos
		+ CameraRight_worldspace * squareVertices.x * BillboardSize.x
		+ CameraUp_worldspace * squareVertices.y * BillboardSize.y;
	*/
	//mats[0] = mats[0] * inv_view;
	/*
	mats[0].m[0] = 1;
	mats[0].m[1] = 0;
	mats[0].m[2] = 0;
	mats[0].m[4] = 0;
	mats[0].m[5] = 1;
	mats[0].m[6] = 0;
	mats[0].m[8] = 0;
	mats[0].m[9] = 0;
	mats[0].m[10] = 1;
	*/
}

void Fire::spinWheels(float delta) {
	rotate_y_sin += 500.0f * delta;
}

