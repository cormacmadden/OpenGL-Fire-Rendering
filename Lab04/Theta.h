#pragma once
#include "maths_funcs.h"
#include <vector>

class Angle
{
	float l1 = 175.0;			//upper arm length
	float l2 = 175.0;			//lower arm length
	float theta_1, theta_2, theta_t1, theta_t2;		//angles
	vec2 source = vec2(700, 550);			//Shoulder Coordinates
	vec3 orient;
	float dist;
	vec3 endPosition;
	vec3 jointApos;
	vec3 jointBpos;
	vec3 jointCpos;
	vec3 target;

public:
	float calcDist(float x1, float y1, float x2, float y2);
	vec2 calcTheta(vec2 dest);
	void jacobian(vec3 endPos);
	vec3 getDeltaOrientation();
	mat3 getJacobianTranspose();
};