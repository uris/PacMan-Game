#pragma once
#include <string>
#include "EnumsAndStatics.h"

using namespace std;

class MainMenu
{

private:
	string** p_menu_options = nullptr;
	string menu_title = "Menu title";
	int menu_options_size = 0;
	int option_selected = -1;
	int exit_index = -1;
	int new_index = -1;
	bool first_draw = true;
	bool show_instructions = true;
	bool is_horizontal = false;
	bool show_icon = false;
	string spacer = "   ";

public:
	//constructor
	MainMenu();

	//destructor
	~MainMenu();

	//methods
	void Create(const string file_name = "PacMan.scenes");
	void Create(const string menu_options, string marker1, string marker2);
	void Create(const string menu_options[], const int menu_options_size, bool return_index);
	void Create(const string menu_options[][2], const int menu_options_size);
	void Create(string** menu_options, const int menu_size);
	void Draw();
	string Show();
	void DeallocateMenuPointer();
	void CreateEmptyArray();
	void Template(MenuTemplates menu_template);
	
	// setters
	void Options(bool show_instructions = true, bool is_horizontal = false, bool show_icon = false, int new_index = -1, int exit_index = -1);
	void Selected(int selected = 0);
	void SetTitle(string title);

	// getters
	string SelectedValue();
	string SelectedOption();
	int SelectedIndex();
	string Title();

};
