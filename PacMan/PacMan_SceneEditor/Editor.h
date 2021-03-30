#pragma once
#include "Cursor.h"
#include "MainMenu.h"
#include "Scene.h"
#include <string>

using namespace std;

class Editor
{

private:
	bool done_editing = false;
	bool exit_editing = false;
	bool is_saved = false;
	bool cancel_edit = false;
	bool is_restart = false;


public:
	Cursor* p_cursor;
	Scene* p_scene;

	//constructor
	Editor();

	//destructor
	~Editor();

	void EditScenes();
	void GetKeyboardInput();
	void SetRefreshDelay();
	int UpdateValue(string option);
	void DoSceneEdit(int load_scene);
	void Reset();
	string ProcessTextOption(string label);
	int ProcessNumberOption(string label);

	// getters
	bool IsDoneEditing();

	// setters
	void SetDoneEditing(bool state);

};
