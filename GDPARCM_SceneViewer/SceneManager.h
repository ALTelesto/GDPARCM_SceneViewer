#pragma once
#include "vector"

class Model3D;

using namespace std;

typedef vector<Model3D*> SceneLocal;
typedef vector<SceneLocal> SceneList;
typedef vector<bool> SceneActive;

class SceneManager
{
public:
	static SceneManager* getInstance();

	void setProgress(int index, float value);
	float getProgress(int index);

	void loadScene(int index, SceneLocal scene);
	SceneLocal getScene(int index);

	void setSceneActive(int index, bool value);
	bool isSceneActive(int index);

	SceneList loadedScenes;

private:
	SceneManager();
	SceneManager(SceneManager const&) {};             // copy constructor is private
	SceneManager& operator=(SceneManager const&) {};  // assignment operator is private
	static SceneManager* sharedInstance;

	vector<float> progress;
	SceneActive sceneActive;
};

