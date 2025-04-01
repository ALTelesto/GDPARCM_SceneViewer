#pragma once

#include "imgui/imgui.h"
#include "SceneManager.h"
#include <string>

class SceneUI
{
private:
    SceneManager* sceneManager;
    const float barWidth = 200.0f;
    const float buttonWidth = 150.0f;
    const float padding = 10.0f;
    const int numScenes = 5;

    void BufferingBar(const char* label, float progress);

public:
    SceneUI();

    ~SceneUI();;

    void render();
};

