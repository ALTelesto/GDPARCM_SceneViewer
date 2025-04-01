#include "SceneUI.h"

void SceneUI::BufferingBar(const char* label, float progress)
{
	ImGui::Text("%s", label);
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
	ImGui::ProgressBar(progress, ImVec2(barWidth, 0.0f));
	ImGui::PopStyleColor();
}

SceneUI::SceneUI()
{
	sceneManager = SceneManager::getInstance();
}

SceneUI::~SceneUI()
{}

void SceneUI::render()
{
	ImGui::Begin("ImGui Screen");

	for (int i = 0; i < numScenes; i++) {
		ImGui::PushID(i);

		ImGui::Text("Scene %d", i + 1);

		if (ImGui::Button("Load Scene", ImVec2(buttonWidth, 0))) {
			//for (float progress = 0.0f; progress <= 1.0f; progress += 0.1f) {
			//	sceneManager->setProgress(i, progress);
			//	// we're supposed to load scene data here but i dont think we have that yet..?
			//	// also i'm coding this while still having linker errors help--
			//}

			// so why not create an empty scene for demonstration
			// SceneLocal scene;
			// sceneManager->loadScene(i, scene);
		}

		ImGui::SameLine(buttonWidth + padding);

		if (ImGui::Button("Unload Scene", ImVec2(buttonWidth, 0))) {
			// idk if this works but theoretically, since we can only show one scene at a time
			// (unless we press the 'load all scenes' button), unloading should return to an empty scene
			/*SceneLocal emptyScene;
			sceneManager->loadScene(i, emptyScene);
			sceneManager->setProgress(i, 0.0f);*/
		}

		bool isActive = sceneManager->isSceneActive(static_cast<sceneviewer::SceneID>(i));
		if (ImGui::Checkbox("Active", &isActive)) {
			/*sceneManager->setSceneActive(i, isActive);*/
		}

		BufferingBar("Progress:", sceneManager->getProgress(static_cast<sceneviewer::SceneID>(i)));

		ImGui::Separator();
		ImGui::PopID();
	}

	if (ImGui::Button("Load All Scenes", ImVec2(2 * buttonWidth + padding, 0))) {
		//for (int i = 0; i < numScenes; i++) {

		//	for (float progress = 0.0f; progress <= 1.0f; progress += 0.1f) {
		//		sceneManager->setProgress(i, progress);
		//		// again we're supposed to load scene data here but i dont think we have that yet..?
		//	}

		//	// now i think we're loading an actual scene here
		//	SceneLocal scene;
		//	sceneManager->loadScene(i, scene);
		//	sceneManager->setSceneActive(i, true);
		//}
	}
    
	ImGui::End();
}
