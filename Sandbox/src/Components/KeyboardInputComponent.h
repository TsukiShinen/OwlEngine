#pragma once
#include <GLFW/glfw3.h>

struct KeyboardInputComponent
{
	int MoveLeft = GLFW_KEY_A;
	int MoveRight = GLFW_KEY_D;
	int MoveForward = GLFW_KEY_W;
	int MoveBackward = GLFW_KEY_S;
	int MoveUp = GLFW_KEY_SPACE;
	int MoveDown = GLFW_KEY_LEFT_CONTROL;
	int LookLeft = GLFW_KEY_LEFT;
	int LookRight = GLFW_KEY_RIGHT;
	int LookUp = GLFW_KEY_UP;
	int LookDown = GLFW_KEY_DOWN;
};
