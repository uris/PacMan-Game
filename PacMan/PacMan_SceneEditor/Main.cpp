#include <iostream>
#include <string>
#include <conio.h>
#include "Draw.h"
#include "Utility.h"

using namespace std;

static constexpr int c_blackwhite = 7; // black bg, white text
static constexpr int c_bluewhite = 30; // blue bg, white text
static constexpr int c_whiteblack = 240; // white bg, black text
static constexpr int c_redwhite = 335; // white bg, black text
int option_selected = -1;
string spacer = "  ";
string menu_options[6];
bool first_draw = true;

// Movement and input keys
static constexpr char kLEFT = 97; // 'a';
static constexpr char kUP = 119; // 'w';
static constexpr char kDOWN = 115; // 's';
static constexpr char kRIGHT = 100; // 'd';
static constexpr char kARROW_UP = 72;
static constexpr char kARROW_DOWN = 80;
static constexpr char kARROW_LEFT = 75;
static constexpr char kARROW_RIGHT = 77;
static constexpr char kESCAPE = 27;
static constexpr char kRETURN = 13;
static constexpr char kN = 110; // 'n'

void DrawOptions();

int main()
{
	//Draw::ShowColors(500);
	Draw::ShowConsoleCursor(false);

	menu_options[0] = "> Scene 1: Act 1";
	menu_options[1] = "> Scene 1: Act 2";
	menu_options[2] = "> Scene 2: Acti 1";
	menu_options[3] = "> Scene 2: Acti 2";
	menu_options[4] = "+ Create a new scene";
	menu_options[5] = "< Exit";

	cout << endl << "   PACMAN SCENE EDITOR" << endl;
	cout << "   Choose a scene you would like to modify or create a new one." << endl;
	cout << "   ----------------- " << endl << endl << endl;

    int input;
	bool return_pressed = false;

    do
    {
		DrawOptions();
		cout << endl << "   ----------------- " << endl;
		cout << "   arrows + return to select.";
		
		input = _getch();

        //if special character then get special key into input
        (input && input == 224) ? input = _getch() : input;

        switch (input)
        {
        case kARROW_UP:
			option_selected >= 1 ? option_selected = option_selected - 1 : option_selected;
            break;
        case kARROW_DOWN:
			option_selected <= 4 ? option_selected = option_selected + 1 : option_selected;
            break;
		case kESCAPE:
			option_selected = 5;
			break;
		case kN:
			option_selected = 4;
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
			if(input - 49 >= 0 && input - 49 <= 5)
				option_selected = input - 48 -1; // 48 = '0', -1 casue array starts at 0
			break;
		case kRETURN:
			return_pressed = true;
			break;
        default:
            break;
        }
    } while (!return_pressed);


	return 0;
}

void DrawOptions()
{

	if (!first_draw)
		Draw::CursorTopLeft(20 - 6);
	first_draw = false;
		

	for (int i = 0; i < 6; i++)
	{
		cout << spacer;
		Draw::SetColor(i == 5 && option_selected == 5 ? c_redwhite : i == 4 && option_selected == 4 ? c_bluewhite : option_selected == i ? c_whiteblack : c_blackwhite);
		cout << " " << menu_options[i] << "  ";
		cout << endl << endl;
		Draw::SetColor(c_blackwhite);
	}
	
}