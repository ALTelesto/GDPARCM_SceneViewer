#pragma once

#include "imgui/imgui.h"
#include "SceneManager.h"
#include <string>

#include "IExecutionEvent.h"
#include "SceneViewerClient.h"

class SceneUI : public IExecutionEvent
{
private:
    SceneManager* sceneManager;
    const float barWidth = 200.0f;
    const float buttonWidth = 150.0f;
    const float padding = 10.0f;
    const int numScenes = 5;

    void BufferingBar(const char* label, float progress);

    std::unordered_map<SceneID,SceneViewerClient*> clients;

    std::vector<bool> shouldUnlock;

public:
    SceneUI();

    ~SceneUI();
    void LoadScene(SceneID index);
    void UnloadScene(SceneID index);;

    void render();

    void onFinishedExecution(int i) override;
};

