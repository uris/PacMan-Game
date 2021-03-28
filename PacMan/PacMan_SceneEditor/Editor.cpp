#include "Editor.h"
#include "MainMenu.h"
#include <iostream>

using namespace std;

//constructor
Editor::Editor() {};

// destructor
Editor::~Editor() {

};

// flow control
void Editor::EditScenes()
{
	// create menu of scenes to edit and display it.
	MainMenu menu;
	menu.Create();
	string option = menu.Show();

	// print the selected option's value
	cout << option;
}