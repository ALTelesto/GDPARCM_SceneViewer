#include "FPSCounter.h"
#include <cstdio>

FPSCounter::FPSCounter() : lastTime(glfwGetTime()), frameCount(0), fps(0.0f) {}

void FPSCounter::render()
{
	double currentTime = glfwGetTime();
	frameCount++;

	// 1 sec interval; can be modified depending on preference
	if (currentTime - lastTime >= 1.0) {
		fps = frameCount / (currentTime - lastTime);
		lastTime = currentTime;
		frameCount = 0;
	}

	ImGui::Begin("FPS Counter");
	ImGui::Text("FPS: %.1f", fps);
	ImGui::End();
}
