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

    do
    {
        // initialize editor variables
        Reset();

        // create menu of scenes to edit, display it and load selected scene.
        menu.Create();
        menu.Template(MenuTemplates::LIST_SCENES);
        string selected_scene = menu.Show();
        
        // if selection is exit or new process that
        if (selected_scene == "#exit")
        {
            is_restart = true; // restart the game
            break; // break out of the base edit loop
        }

        int load_scene;
        if (selected_scene == "#new")
            load_scene = 0;
        else
            load_scene = stoi(selected_scene);
        
        // do the editing of the selcted scene if not exiting
        DoSceneEdit(load_scene);

        // check cancel - restart edit loop if cancel is true
        if (cancel_edit) {
            system("cls");
            continue;
        }

        // cls
        system("cls");

        // process the selected option, setting the value or saving the scene edits
        string selected_option = "#none";

        while (selected_option != "#save")
        {
            // show the menu of options
            options.Create(p_scene->GetSceneValues(), 11);
            options.Template(MenuTemplates::EDIT_SCENE_OPTIONS);
            string selected_option = options.Show();

            // process the selected option
            if (selected_option == "#save")
            {
                // do save stuff
                p_scene->SaveToFile();
                is_saved = true;
                break;
            }
            else
            {
                // update the scene option values
                UpdateValue(selected_option);
            }
           
            // clear
            system("cls");
        }

    } while (!is_saved);

    system("cls");
    cout << "ended edit.";
    

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
            p_cursor->SetDirection(Direction::LEFT);
            break;
        case Globals::kARROW_RIGHT:
            p_cursor->SetDirection(Direction::RIGHT);
            break;
        case Globals::kARROW_UP:
            p_cursor->SetDirection(Direction::UP);
            break;
        case Globals::kARROW_DOWN:
            p_cursor->SetDirection(Direction::DOWN);
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
            if (done_editing) {
                cancel_edit = false;
                done_editing = true;
                exit_editing = true;
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
                else
                {
                    p_cursor->ChangeMode();
                }
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
        case '#':
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
    system("cls"); Draw::ShowConsoleCursor(true);
    option = Utility::TransformString(option, 1);
    string spacer = "  ";

    if (option == "0") { //scen #
        p_scene->this_scene = ProcessNumberOption(spacer +"Enter the new scene number : ");
        return 0;
    }

    if (option == "1") { // title
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

    if (option == "5") { // run duration
        p_scene->edible_ghost_duration = ProcessNumberOption(spacer + "Edible ghost duration: ");
        return 0;
    }

    if (option == "6") { // chase duration
        p_scene->chase_for = ProcessNumberOption(spacer + "Chase duration: ");
        return 0;
    }

    if (option == "7") { // chase duration
        p_scene->run_for = ProcessNumberOption(spacer + "Run duration: ");
        return 0;
    }

    if (option == "8") { // roam duration
        p_scene->roam_for = ProcessNumberOption(spacer + "Roam duration: ");
        return 0;
    }

    if (option == "9") { // roam count
        p_scene->roam_count = ProcessNumberOption(spacer + "Roam count: ");
        return 0;
    }

    return 0;
}

void Editor::DoSceneEdit(int load_scene)
{
    // clear screen
    system("cls");

    // Create the scene based on the selection
    p_scene->CreateLevelScene(load_scene);
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
    cout << label;
    getline(cin, string_input);
    return string_input;
}

int Editor::ProcessNumberOption(string label)
{
    int int_input;
    cout << label;
    cin >> int_input;
    return int_input;
}
