#include "SceneUI.h"

#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

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
	for(int i = 0; i < numScenes; i++)
	{
		SceneID index = static_cast<SceneID>(i);
		clients.insert(make_pair(index, nullptr));
		this->shouldUnlock.push_back(false);
	}
}

SceneUI::~SceneUI()
{}

void SceneUI::LoadScene(SceneID index)
{
	if (SceneManager::getInstance()->registerLoadScene(index))
	{
		SceneViewerClient* client = new SceneViewerClient(index, LOAD, grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
		this->clients[index] = client;
		client->setExecutionEvent(this);
		client->start();
	}
}

void SceneUI::UnloadScene(SceneID index)
{
	if (SceneManager::getInstance()->registerLoadScene(index))
	{
		SceneViewerClient* client = new SceneViewerClient(index, UNLOAD, grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
		this->clients[index] = client;
		client->setExecutionEvent(this);
		client->start();
	}
}

void SceneUI::render()
{
	ImGui::Begin("ImGui Screen");

	for (int i = 0; i < numScenes; i++) {
		SceneID index = static_cast<SceneID>(i);
		bool disabled = false;

		ImGui::PushID(i);

		ImGui::Text("Scene %d", i + 1);

		if (!SceneManager::getInstance()->getScene(index).empty() || SceneManager::getInstance()->getProgress(index) > 0.0f)
		{
			ImGui::BeginDisabled();
			disabled = true;
		}
		if (ImGui::Button("Load Scene", ImVec2(buttonWidth, 0))) {
			LoadScene(index);
		}
		if(disabled)
		{
			ImGui::EndDisabled();
			disabled = false;
		}

		ImGui::SameLine(buttonWidth + padding);


		if (SceneManager::getInstance()->getProgress(index) != 1.0f)
		{
			ImGui::BeginDisabled();
			disabled = true;
		}
		if (ImGui::Button("Unload Scene", ImVec2(buttonWidth, 0))) {
			UnloadScene(index);
		}
		if (disabled)
		{
			ImGui::EndDisabled();
			disabled = false;
		}

		if (SceneManager::getInstance()->getScene(index).empty() && SceneManager::getInstance()->getProgress(index) != 1.0f)
		{
			ImGui::BeginDisabled();
			disabled = true;
		}
		bool isActive = sceneManager->isSceneActive(static_cast<sceneviewer::SceneID>(i));
		if (ImGui::Checkbox("Active", &isActive)) {
			sceneManager->setSceneActive(index, isActive);
		}
		if (disabled)
		{
			ImGui::EndDisabled();
		}

		BufferingBar("Progress:", sceneManager->getProgress(static_cast<sceneviewer::SceneID>(i)));

		ImGui::Separator();
		ImGui::PopID();
	}

	if (ImGui::Button("Load All Scenes", ImVec2(2 * buttonWidth + padding, 0))) {
		for(int i = 0; i < numScenes; i++)
		{
			SceneID index = static_cast<SceneID>(i);
			if(SceneManager::getInstance()->getScene(index).empty() && SceneManager::getInstance()->getProgress(index) != 1.0f)
			{
				LoadScene(index);
			}
			else
			{
				sceneManager->setSceneActive(index, true);
			}
		}
	}
    
	ImGui::End();

	for(int i = 0; i < numScenes; i++)
	{
		if(this->shouldUnlock[i] == true)
		{
			SceneID index = static_cast<SceneID>(i);
			std::cout << "[SceneUI] Releasing index " + to_string(i) << std::endl;
			SceneManager::getInstance()->reportDoneScene(index);
			this->shouldUnlock[i] = false;
		}
	}
}

void SceneUI::onFinishedExecution(int i)
{
	this->shouldUnlock[i] = true;
}
