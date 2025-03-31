#pragma once

class Scene
{
public:

};

class SceneManager
{
public:
	static SceneManager* getInstance();
private:
	SceneManager();
	SceneManager(SceneManager const&) {};             // copy constructor is private
	SceneManager& operator=(SceneManager const&) {};  // assignment operator is private
	static SceneManager* sharedInstance;
};

