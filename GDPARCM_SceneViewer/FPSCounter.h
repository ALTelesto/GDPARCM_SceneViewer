#pragma once

#include <GLFW/glfw3.h>
#include "imgui/imgui.h"

class FPSCounter
{
public:
	FPSCounter();
	void render();

private:
	double lastTime;
	int frameCount;
	float fps;
};

