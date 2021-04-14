#include "Editor.h"
#include "Utility.h"
#include "Draw.h"
#include <iostream>
#include <conio.h>
#include <thread>
#include <chrono>

using namespace std;

//constructor
Editor::Editor()
{
    p_cursor = new Cursor();
    p_scene = new Scene();
    p_cursor->SetEditor(this);
    p_scene->SetEditor(this);
};

// destructor
Editor::~Editor()
{
    if (p_cursor)
    {
        delete p_cursor;
        p_cursor = nullptr;
    }
    if (p_scene)
    {
        delete p_scene;
        p_scene = nullptr;
    }
    
};

// flow control
void Editor::EditScenes()
{
    MainMenu menu;
    MainMenu options;
    bool new_scene = false;

    do
    {
        // initialize editor variables
        Reset();
        new_scene = false;

        // create menu of scenes to edit, display it and load selected scene.
        menu.Create();
        menu.Template(MenuTemplates::LIST_SCENES);
        string selected_scene = menu.Show();
        
        // if selection is exit then break out
        if (selected_scene == "#exit")
        {
            is_restart = true; // restart the game
            break; // break out of the base edit loop
        }

        // process the edit selection
        int load_scene;

        if (selected_scene == "#new")
        {
            // list the available scenes as temmplate scenes
            system("cls");
            menu.Create("PacMan.scenes", true);
            menu.Template(MenuTemplates::LIST_TEMPLATES);
            string selected_template = menu.Show();
            
            if (selected_template == "#new")
            {
                load_scene = 0;
                new_scene = true;
            }
            else if (selected_template == "#exit")
            {
                is_restart = true;
                break;
            }
            else
            {
                load_scene = stoi(selected_template);
                new_scene = true;
            }
        }   
        else
        {
            // load the selected existing scene
            load_scene = stoi(selected_scene);
        }
            
        
        // do the editing of the selcted scene if not exiting
        
        // set console font to pacman font
        Draw::SetConsoleFont(true, Resolution::NORMAL);
        do
        {
            DoSceneEdit(load_scene);
            if (new_scene)
            {
                p_scene->this_scene = 0;
                new_scene = false;
            }
        } while (!exit_editing);
        
        // set console font back to default
        Draw::SetConsoleFont(false, Resolution::NORMAL);
        Draw::SetConsoleSize(Resolution::NORMAL);

        // check cancel - restart edit loop if cancel is true
        if (cancel_edit) {
            std::system("cls");
            p_scene->DeallocateMapArray();
            continue;
        }

        // cls
        std::system("cls");

        // process the selected option, setting the value or saving the scene edits
        string selected_option = "#none";

        while (selected_option != "#save")
        {
            // show the menu of options
            options.Create(p_scene->GetSceneValues(), 12);
            options.Template(MenuTemplates::EDIT_SCENE_OPTIONS);
            string selected_option = options.Show();

            // process the selected option
            if (selected_option == "#save")
            {
                // do save
                p_scene->SaveToFile();
                p_scene->DeallocateMapArray();
                is_saved = true;
                break;
            }
            if (selected_option == "#cancel")
            {
                // cancel edit and return to main menu
                system("cls");
                p_scene->DeallocateMapArray();
                is_saved = false;
                break;
            }
            else
            {
                // update the scene option values
                UpdateValue(selected_option);
            }
           
            // clear
            std::system("cls");
        }

    } while (!is_saved);

    std::system("cls");

    if (is_saved) {
        cout << endl << endl;
        cout << "   PACMAN SCENE EDITOR" << endl;
        cout << "   The Scene was saved successfuly." << endl;
        cout << "   Press any key to continue." << endl;
        int input = _getch();
    }
    
    Reset();
    std::system("cls");
}

void Editor::GetKeyboardInput()
{
    int input;

    do
    {
        input = _getch();

        //if special character then get special key into input
        (input && input == 224) ? input = _getch() : input;

        switch (input)
        {
        case Globals::kARROW_LEFT:
            if (!p_scene->resize_scene)
                p_cursor->SetDirection(Direction::LEFT);
            else
                if(!p_scene->remove_cols)
                    p_scene->remove_cols = true;
            break;
        case Globals::kARROW_RIGHT:
            if (!p_scene->resize_scene)
                p_cursor->SetDirection(Direction::RIGHT);
            else
                if (!p_scene->add_cols)
                    p_scene->add_cols = true;
            break;
        case Globals::kARROW_UP:
            if (!p_scene->resize_scene)
                p_cursor->SetDirection(Direction::UP);
            else
                if (!p_scene->remove_rows)
                    p_scene->remove_rows = true;
            break;
        case Globals::kARROW_DOWN:
            if (!p_scene->resize_scene)
                p_cursor->SetDirection(Direction::DOWN);
            else
                if (!p_scene->add_rows)
                    p_scene->add_rows = true;
            break;
        case Globals::kSPACE:
            if (p_cursor->IsEditing()) {
                if (p_cursor->Pen() == '0')
                {
                    p_cursor->SetPen(' ');
                }
                else
                {
                    p_cursor->ChangeMode(false);
                }
            }
            else
            {
                p_cursor->ChangeMode(false);
            }
            break;
        case 'y':
            if (done_editing)
            {
                if (p_scene->ValiditeScene())
                {
                    cancel_edit = false;
                    done_editing = true;
                    exit_editing = true;
                }
                else
                {
                    cancel_edit = false;
                    done_editing = false;
                    exit_editing = false;
                }
            }
            break;
        case 'n':
            if (done_editing) {
                cancel_edit = true;
                done_editing = true;
                exit_editing = true;
                break;
            }
        case Globals::kESCAPE:
            if (p_cursor->IsEditing()) {
                if (p_cursor->Pen() != '0')
                {
                    p_cursor->SetPen('0');
                }
                else if (p_scene->resize_scene == true)
                {
                    done_editing = true;
                }
                else
                {
                    p_cursor->ChangeMode();
                }
            }
            else if (p_scene->resize_scene)
            {
                p_scene->resize_scene = false;
                p_cursor->SetPen('0');
                p_cursor->ChangeMode();
            }
            else if (done_editing)
            {
                done_editing = false;
            }
            else
            {
                done_editing = true;
            }
            break;
        case Globals::kRETURN:
            if (p_cursor->pen_is_walls)
                p_cursor->CycleLongWalls();
            if (p_cursor->pen_is_short_walls)
                p_cursor->CycleShortWalls();
            if (p_cursor->pen_is_fruits)
                p_cursor->CycleFruits();
            break;
        case Globals::kEQUALS:
            if (p_cursor->pen_is_walls)
                p_cursor->CycleLongWalls();
            if (p_cursor->pen_is_short_walls)
                p_cursor->CycleShortWalls();
            if (p_cursor->pen_is_fruits)
                p_cursor->CycleFruits();
            break;
        case Globals::kMINUS:
            if (p_cursor->pen_is_walls)
                p_cursor->CycleLongWalls(false);
            if (p_cursor->pen_is_short_walls)
                p_cursor->CycleShortWalls(false);
            if (p_cursor->pen_is_fruits)
                p_cursor->CycleFruits(false);
            break;
        case Globals::kHASH:
        case Globals::kAT:
        case Globals::kF:
        case '%':
        case '^':
        case '.':
        case 'o':
        case 't':
        case 's':
            if (p_cursor->IsEditing()) {
                p_cursor->SetPen(char(input));
            }
            break;
        case 'r':
            if (p_cursor->IsEditing() && p_cursor->Pen() == '0' && !p_scene->resize_scene) {
                p_cursor->ChangeMode();
                p_scene->resize_scene = true;
                //exit_editing = true;
            }
            break;
        default:
            break;
        }
    } while (!exit_editing);
}

void Editor::SetRefreshDelay()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(Globals::editor_refresh_delay)); // refresh rate using pause before next cycle
}

bool Editor::IsDoneEditing()
{
    return done_editing;
}

void Editor::SetDoneEditing(bool state)
{
    done_editing = state;
}

int Editor::UpdateValue(string option)
{
    std::system("cls"); Draw::ShowConsoleCursor(true);
    option = Utility::TransformString(option, 1);
    string spacer = "  ";

    if (option == "0") { //scen #
       // p_scene->this_scene = Utility::ProcessOption<int>("Enter the new scene number : ");
       p_scene->this_scene = ProcessNumberOption(spacer +"Enter the new scene number : ");
        return 0;
    }

    if (option == "1") { // title
        //p_scene->title = Utility::ProcessOption<string, false>("Enter the new title: ");
        p_scene->title = ProcessTextOption(spacer + "Enter the new title: ");
        return 0;
    }

    if (option == "2") { // points per pellet
        p_scene->points_pellet = ProcessNumberOption(spacer + "Points per pellet: ");
        return 0;
    }

    if (option == "3") { // points per ghost
        p_scene->points_ghost = ProcessNumberOption(spacer + "Points per ghots: ");
        return 0;
    }

    if (option == "4") { // points all ghosts
        p_scene->all_ghost_bonus = ProcessNumberOption(spacer + "Points for all ghots: ");
        return 0;
    }

    if (option == "5") { // chase duration
        p_scene->chase_for = ProcessNumberOption(spacer + "Chase duration: ");
        return 0;
    }

    if (option == "6") { // chase duration
        p_scene->run_for = ProcessNumberOption(spacer + "Run duration: ");
        return 0;
    }

    if (option == "7") { // roam duration
        p_scene->roam_for = ProcessNumberOption(spacer + "Roam duration: ");
        return 0;
    }

    if (option == "8") { // roam count
        p_scene->roam_count = ProcessNumberOption(spacer + "Roam count: ");
        return 0;
    }

    if (option == "9") { // fuit value
        p_scene->fruit_points = ProcessNumberOption(spacer + "Points for fruit: ");
        return 0;
    }

    return 0;
}

void Editor::DoSceneEdit(int load_scene)
{
    // clear screen
    std::system("cls");

    // Create the scene based on the selection
    if (!p_scene->p_map)
    {
        p_scene->CreateLevelScene(load_scene);
    }
    p_scene->DrawLevel();

    // Get keyboard entry for editing scene
    thread inputThread(&Editor::GetKeyboardInput, this); // new the ref() wrapper to pass by ref into thread

    // Edit the scene while not done editing
    do
    {
     
            // move the cursor
            p_cursor->MoveCursor();

            //draw the edited scene
            p_scene->DrawLevel();

            // refresh rate to slow down render
            SetRefreshDelay();
        

    } while (!exit_editing);

    // join the keyboard thread back to the main thread
    inputThread.join();

    
}

void Editor::Reset()
{
    // reset editing state
    done_editing = false;
    exit_editing = false;
    is_saved = false;
    cancel_edit = false;
}

string Editor::ProcessTextOption(string label)
{
    string string_input;
    
    do
    {
        cout << endl;
        cout << label;
        getline(cin, string_input);
        
        if (string_input.length() < 1) {
            cout << "  You need a title to save your scene!" << endl;
            cout << "  Press a any key to continue.";
            int input = _getch();
            system("cls");
        }
        else
        {
            break;
        }

    } while (true);
    
    return string_input;
}

int Editor::ProcessNumberOption(string label)
{
    int int_input;

    do
    {
        cout << endl;
        cout << label;
        cin >> int_input;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  You need to enter a vlaid number!" << endl;
            cout << "  Press a any key to continue.";
            int input = _getch();
            system("cls");
        }
        else
        {
            break;
        }

    } while (true);

    return int_input;
}
