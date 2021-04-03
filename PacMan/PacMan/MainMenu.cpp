#include "MainMenu.h"
#include <iostream>
#include "Utility.h"
#include "Draw.h"
#include <conio.h>

using namespace std;

//constructor
MainMenu::MainMenu() {};

// destructor
MainMenu::~MainMenu() {

	// clean up options array
	DeallocateMenuPointer();

};

// methods
void MainMenu::DeallocateMenuPointer()
{
	// clean up options array
	if (p_menu_options)
	{
		for (int i = 0; i < menu_options_size; i++)
		{
			delete[] p_menu_options[i];
		}
		delete[] p_menu_options;
		p_menu_options = nullptr;
	}
}

void MainMenu::Create(const string file_name, const bool templates)
{
	// if there's a pointer array already, delete it
	DeallocateMenuPointer();
	
	size_t pos = 0; 
	string marker = "\n";
	string options = Utility::GetMenuFromFile(file_name); // get string from file

	// get number of options for size of the menu array
	if ((pos = options.find(marker)) != std::string::npos)
	{
		menu_options_size = stoi(options.substr(0, pos)) + 2;
	}
	options.erase(0, pos + marker.length()); // remove first line that contains size info

	// create the empty pointer array
	CreateEmptyArray();

	// shove the options into the menu array
	for (int i = 0; i < menu_options_size - 2; i++)
	{
		if ((pos = options.find("*")) != std::string::npos)
		{
			p_menu_options[i][0] = options.substr(0, pos);
			options.erase(0, pos + marker.length());
		}
		
		if ((pos = options.find("\n")) != std::string::npos)
		{
			p_menu_options[i][1] = options.substr(0, pos);
			options.erase(0, pos + marker.length());
		}

	}
	p_menu_options[menu_options_size - 2][0] = "#new";
	p_menu_options[menu_options_size - 2][1] = (templates ? "Blank template" : "Create a new scene");
	p_menu_options[menu_options_size - 1][0] = "#exit";
	p_menu_options[menu_options_size - 1][1] = "Exit";

	exit_index = menu_options_size - 1;
	new_index = menu_options_size - 2;

}

void MainMenu::Create(string menu_options, string marker1, string marker2)
{
	// if there's a pointer array already, delete it
	DeallocateMenuPointer();

	size_t pos = 0;

	// get number of options for size of the menu array
	if ((pos = menu_options.find(marker1)) != std::string::npos)
	{
		menu_options_size = stoi(menu_options.substr(0, pos));
	}
	menu_options.erase(0, pos + marker1.length()); // remove first line that contains size info

	// create the empty pointer array
	CreateEmptyArray();

	// shove the options into the menu array
	for (int i = 0; i < menu_options_size; i++)
	{
		if ((pos = menu_options.find(marker2)) != std::string::npos)
		{
			p_menu_options[i][0] = menu_options.substr(0, pos);
			menu_options.erase(0, pos + marker2.length());
		}

		if ((pos = menu_options.find("\n")) != std::string::npos)
		{
			p_menu_options[i][1] = menu_options.substr(0, pos);
			menu_options.erase(0, pos + marker1.length());
		}

	}

}

void MainMenu::Create(const string menu_options[], const int menu_size, bool return_index)
{
	DeallocateMenuPointer();
	menu_options_size = menu_size;
	CreateEmptyArray();

	// shove the options into the menu array
	for (int i = 0; i < menu_options_size; i++)
	{
		p_menu_options[i][0] = menu_options[i];
		p_menu_options[i][1] = return_index ? to_string(i) : menu_options[i];
	}

}

void MainMenu::Create(const string menu_options[][2], const int menu_size)
{
	DeallocateMenuPointer();
	menu_options_size = menu_size;
	CreateEmptyArray();

	// shove the options into the menu array
	for (int i = 0; i < menu_options_size; i++)
	{
		p_menu_options[i][0] = menu_options[i][0];
		p_menu_options[i][1] = menu_options[i][1];
	}
}

void MainMenu::Create(string** menu_options, const int menu_size)
{
	DeallocateMenuPointer();
	menu_options_size = menu_size;
	CreateEmptyArray();

	// shove the options into the menu array
	for (int i = 0; i < menu_options_size; i++)
	{
		p_menu_options[i][0] = menu_options[i][0];
		p_menu_options[i][1] = menu_options[i][1];
	}
}

void MainMenu::CreateEmptyArray()
{
	p_menu_options = new string * [menu_options_size]; //  set the dynamic array to the menu size size
	for (int i = 0; i < menu_options_size; i++)
	{
		p_menu_options[i] = new string[2];
	}

	exit_index = -1;
	new_index = -1;
}

void MainMenu::Draw()
{
	Draw::ShowConsoleCursor(false);
	
	if (!first_draw)
		if (!is_horizontal)
			Draw::CursorTopLeft((menu_options_size * 2) + (show_instructions ? 2 : 0));
		else
			Draw::CursorTopLeft((show_instructions ? 2 : 0));
	first_draw = false;
	
	string icon = "";
	
	if (is_horizontal) {
		cout << spacer;
	}

	for (int i = 0; i < menu_options_size; i++)
	{
		
		if (!is_horizontal) {
			cout << spacer;
		}
		
		Draw::SetColor(option_selected == i ? Globals::c_whiteblack : Globals::c_blackwhite);
		
		if (show_icon)
		{
			icon = "> ";
			i == exit_index ? icon = "< " : icon;
			i == new_index ? icon = "+ " : icon;
		}

		cout << (icon + p_menu_options[i][1]) + " ";

		Draw::SetColor(Globals::c_blackwhite);
		if (!is_horizontal)
		{
			cout << endl << endl;
		}
		else
		{
			cout << "  ";
		}
		
	}
}

string MainMenu::Show()
{
	Draw::ShowConsoleCursor(false);

	cout << menu_title;

	int input;
	bool return_pressed = false;

	do
	{
		Draw();
		
		if (show_instructions)
		{
			cout << endl << "   ----------------- " << endl;
			cout << "   arrows + return to select.";
		}
		

		input = _getch();

		//if special character then get special key into input
		(input && input == 224) ? input = _getch() : input;

		switch (input)
		{
		case Globals::kARROW_LEFT:
		case Globals::kARROW_UP:
			option_selected >= 1 ? option_selected = option_selected - 1 : option_selected;
			break;
		case Globals::kARROW_RIGHT:
		case Globals::kARROW_DOWN:
			option_selected < menu_options_size-1 ? option_selected = option_selected + 1 : option_selected;
			break;
		case Globals::kESCAPE:
			option_selected = exit_index;
			break;
		case Globals::kN:
			option_selected = new_index;
			break;
		case 49:
		case 50:
		case 51:
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
			if (input - 49 >= 0 && input - 49 <= menu_options_size -1)
				option_selected = input - 48 - 1; // 48 = '0', -1 casue array starts at 0
			break;
		case Globals::kRETURN:
			if (option_selected > -1) {
				return_pressed = true;
			}
			break;
		default:
			break;
		}
	} while (!return_pressed);

	// so that next time the menu is show it gets reset
	first_draw = true;
	
	return (p_menu_options[option_selected][0]);
}

// setters
void MainMenu::Options(bool show_instructions, bool is_horizontal, bool show_icon, int new_index, int exit_index)
{
	new_index == -1 ? this->new_index = this->new_index : this->new_index = new_index;
	exit_index == -1 ? this->exit_index = this->exit_index : this->exit_index = exit_index;
	this->show_instructions = show_instructions;
	this->is_horizontal = is_horizontal;
	this->show_icon = show_icon;

}

void MainMenu::Selected(int selected)
{
	option_selected = selected - 1;
}

// getters

string MainMenu::SelectedValue()
{
	return p_menu_options[option_selected][1];
}

string MainMenu::SelectedOption()
{
	return p_menu_options[option_selected][0];
}

int MainMenu::SelectedIndex()
{
	return option_selected;
}

void MainMenu::SetTitle(string title)
{
	menu_title = title;
}

string MainMenu::Title()
{
	return menu_title;
}

void MainMenu::Template(MenuTemplates menu_template)
{
	switch (menu_template)
	{
	case MenuTemplates::EDIT_SCENE_OPTIONS:
		is_horizontal = false;
		show_instructions = false;
		show_icon = true;
		new_index = 9;
		exit_index = 10;
		first_draw = true;
		option_selected = -1;
		menu_title =  "\n   EDIT SCENE OPTIONS\n";
		menu_title += "   Choose an option to modify\n";
		menu_title += "   Select save to comitt changes.\n";
		menu_title += "   ----------------- \n\n";
		break;
	case MenuTemplates::CHOOSE_EDIT_PLAY:
		is_horizontal = true;
		show_instructions = false;
		show_icon = false;
		new_index = -1;
		exit_index = -1;
		first_draw = true;
		option_selected = 0;
		menu_title = "";
		spacer = "    ";
		break;
	case MenuTemplates::LIST_SCENES:
		is_horizontal = false;
		show_instructions = true;
		show_icon = true;
		option_selected = -1;
		menu_title =  "\n   PACMAN SCENE EDITOR\n";
		menu_title += "   Choose a scene you would like to modify\n";
		menu_title += "   Or create a new one from a template.\n";
		menu_title += "   ----------------- \n\n";
		break;
	case MenuTemplates::LIST_TEMPLATES:
		is_horizontal = false;
		show_instructions = true;
		show_icon = true;
		option_selected = -1;
		menu_title = "\n   SCENE TEMPLATES\n";
		menu_title += "   Choose a scene to use as a template\n";
		menu_title += "   Start from scratch with a blank template.\n";
		menu_title += "   ----------------- \n\n";
		break;
	default: // case none
		is_horizontal = false;
		show_instructions = true;
		show_icon = false;
		new_index = -1;
		exit_index = -1;
		option_selected = -1;
		menu_title = "";
		break;
	}
}