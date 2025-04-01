#pragma once
#include "vector"
#include "mutex"
#include "sceneviewer.pb.h"

class Model3D;

using namespace std;
using namespace sceneviewer;

typedef vector<Model3D*> SceneLocal;
typedef vector<SceneLocal> SceneList;
typedef vector<bool> SceneActive;

class SceneManager
{
public:
	static SceneManager* getInstance();

	void setProgress(SceneID index, float value);
	float getProgress(SceneID index);

	void loadScene(SceneID index, SceneLocal scene);
	SceneLocal getScene(SceneID index);

	void setSceneActive(SceneID index, bool value);
	bool isSceneActive(SceneID index);

	bool registerLoadScene(SceneID index);
	void reportDoneScene(SceneID index);

	SceneList loadedScenes;

private:
	SceneManager();
	SceneManager(SceneManager const&) {};             // copy constructor is private
	SceneManager& operator=(SceneManager const&) {};  // assignment operator is private
	static SceneManager* sharedInstance;

	vector<float> progress;
	SceneActive sceneActive;

	mutex sceneGuard;
	mutex progressGuard;
	vector<mutex*> loadingGuard;
};

