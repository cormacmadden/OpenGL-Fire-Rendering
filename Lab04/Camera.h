#pragma once
#define _USE_MATH_DEFINES

// Project includes
#include "maths_funcs.h"
#include "Utils.h"
//#include "Paperboy.h"

// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <vector>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 20.0f;
const float SENSITIVITY = 0.4f; //mouse is not very sensitive 

class Camera
{
public:
    vec3 Pos;
    vec3 Front;
    vec3 Up;
    vec3 Right;
    vec3 WorldUp;
    float Yaw;
    float Pitch = -70.0f;
    float MovementSpeed;
    float MouseSensitivity;
    bool thirdPerson = false;
    bool originalCamera = false;
    bool freeCam = true;
    float Fov = 45.0f;


    Camera::Camera(vec3 position = vec3(0.0f, 5.0f, 20.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
    {
        Pos = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCamVectors();
    }
    //function to return cameras viewpoint as mat4
    mat4 GetViewMatrix();

    //function to handle change of cam position due to keyboard input 
    void ProcessKeyboard(MovementDir direction, float deltaTime);

    void processThirdPerson(vec3 position, vec3 direction);

    //function to allow camera to move with mouse pos
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);

    //function to convert degrees to radians
    float convert(float degree);

    void processFirstPerson(vec3 position, vec3 direction);
private:

    //function to update cam's vectors 
    void updateCamVectors();
    void updateCamVectorsNoFront();

};