#include <fstream> // file manipulation
#include <iostream> // print to console
#include <stdlib.h> // exit
#include <conio.h> // used to get input without requiring return
#include "Scene.h"
#include "Utility.h"
#include "Draw.h"
#include "Editor.h"
#include <chrono>
#include <thread>

using namespace std;

//constructors
Scene::Scene()
{
	// do nothing for now
}

//destructors
Scene::~Scene()
{
    // deallcate options array
    DeleteOptions();
    
    // deallocated map array
    DeallocateMapArray();

    // nullify editor pointer
    if (p_editor)
    {
        p_editor = nullptr;
    }
}

// Public Methods
std::ostream& operator<<(std::ostream& left, const Coord right)
{
    left << "{" << right.row << "," << right.col << "}";
    return left;
}

void Scene::CreateLevelScene(int& current_scene)
{
    // load level info and map from scenes file using games current level
    string map = LoadSceneFromFile("PacMan.scenes", current_scene);

    // if the string return "false" then you've reached the end of the game
    if (map == "false") {
        system("cls");
        cout << "You beat the PC! Good for you" << endl;
        system("pause");
        exit(0);
    }

    // parse through string to replace pellt and powerup markers to their ascii code
    Utility::ReplaceString(map, ".", char(Globals::pellet));

    // replace the fruit marker with the fruit character
    Utility::ReplaceString(map, "F", GetFruitChar());

    // create dynamic two dimension pointer array to hold map
    Coord size = MapSize(map); // get width and height of the map
    cols = size.col; // set lsizes in level
    rows = size.row; // set sizes in level

    char** p_mapArray = new char* [size.row];
    for (int i = 0; i < size.row; i++)
    {
        p_mapArray[i] = new char[size.col];
    }

    int row = 0, col = 0, index = 0;

    // iterate through the characters of the map string
    for (string::size_type i = 0; i < map.size(); i++)
    {
        row = abs((int)i / size.col);
        col = size.col - ((((row + 1) * size.col) - (int)i));

        // add to dynamic map array
        p_mapArray[row][col] = map[i];

    }

    // return map array pointer
    p_map = p_mapArray;
}

string Scene::LoadSceneFromFile(string filename, int scene_to_load)
{
    
    ifstream scenesFile(filename);
    string fileLine, section, map = "";
    bool processLines = true;

    if (scenesFile) {
        // file exists and is open 
        while (getline(scenesFile, fileLine))
        {

            //check for right scene
            section = "#scene:";
            if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) { // if line is scene identifier

                if (scene_to_load == stoi(fileLine.substr(Utility::TransformString(section, 1).size(), 2))) // and if the sence is the one I'm looking for
                {
                    
                    // set the scene number
                    this_scene = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), 2));
                    
                    //this is the scene we are looking for so let's process it until your get to the end of the level info
                    while (getline(scenesFile, fileLine))
                    {
                        // get the next line and set the relevant info in the level info

                        section = "title:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            title = fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size()));
                            continue;
                        }

                        section = "pellet_points:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            points_ghost = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "ghost_points:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            points_ghost = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "all_ghosts_bonus:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            all_ghost_bonus = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "chase_duration:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            chase_for = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "run_duration:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            run_for = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "roam_duration:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            roam_for = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "roam_count:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            roam_count = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "fruit:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            fruit = GetFruitType(fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "fruit_points:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            fruit_points = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "level_map:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            // get map info into the map string ending when you reach end of map
                            section = "#end_scene";
                            while (getline(scenesFile, fileLine))
                            {
                                if (fileLine.find(Utility::TransformString(section, 1), 0) == std::string::npos)
                                    map += fileLine;
                                else
                                    break;
                            }
                        }

                        section = "#scene:"; // check to see if reached the next scene;
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            break;
                        }

                    }
                }
                processLines = false;
            }
        }
        scenesFile.close();
    }
    else
    {
        //unable to read file - write sine error code. For now not polished.
        system("cls");
        cout << "Can't seem to load the PacMan scenes. Check your code man!" << endl;
        system("pause");
        exit(0);
    }

    return (map.size() > 0 ? map : "false");
}

Coord Scene::MapSize(const string& map)
{
    size_t rows = 0, cols = 0;
    size_t pos;

    pos = map.find(char(Globals::corner_marker));
    if (pos != string::npos)
    {
        pos = map.find(char(Globals::corner_marker), pos + 1);
        if (pos != string::npos)
        {
            cols = pos + 1;
            rows = map.length() / cols;
            /*pos = map.find("+", pos + 1);
            if (pos != string::npos) {
                rows = (pos / cols) + 1;
            }*/
        }
    }
    return { (int)rows, (int)cols };
}

void Scene::DrawLevel()
{
    // set console size to accomodayte changes in rows/cols
    Draw::SetConsoleSize(Resolution::NORMAL, rows + 11, max(cols +4, 35));
    
    // remove cursor from screen to avoid the flicker
    Draw::ShowConsoleCursor(false);
    
    // place cursor on top left of console
    Draw::CursorTopLeft(rows + top_left_modifier); // + x for cpations

    // update width and height of the level
    ResizeScene();
  
    // Level Title
    string format = Utility::Spacer(" PACMAN: " + Utility::TransformString(title, 0), cols);
    Draw::SetColor(Globals::cWHITE);
    cout << endl;
    cout << format << "PACMAN: " + Utility::TransformString(title, 0) << format;
    cout << endl << Draw::WriteEmptyLine(31) << endl;

    // remove cursor and replace with current content of the cursor
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            if (p_map[r][c] == char(Globals::cursor))
            {
                if (p_editor->p_cursor->Pen() == '0' || !p_editor->p_cursor->IsEditing())
                {
                    p_map[r][c] = p_editor->p_cursor->content_now;
                }
                else
                {
                    p_map[r][c] = p_editor->p_cursor->Pen();
                }
               
            }
        }
    }

    // check pen is valid (will set to '.' if not)
    if (!PenIsValid(p_editor->p_cursor->Pen()))
    {
        p_editor->p_cursor->SetPen('.');
    }

    // draw current level map
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {

            // if this is the cursor current pos place it there
            if (p_editor->p_cursor->GetCurrentPosition() == Coord{ r,c })
            {
                p_editor->p_cursor->content_now = p_map[p_editor->p_cursor->GetCurrentRow()][p_editor->p_cursor->GetCurrentCol()];
                p_map[r][c] = char(Globals::cursor);
            }
                
            
            // set color of map content at this position
            
            char print; // will hold the output char

            switch (p_map[r][c])
            {
            
            //long walls
            case char(Globals::lwall_184):
            case char(Globals::lwall_187):
            case char(Globals::lwall_188):
            case char(Globals::lwall_200):
            case char(Globals::lwall_203):
            case char(Globals::lwall_209):
            case char(Globals::lwall_212):
            case char(Globals::lwall_213):
            case char(Globals::lwall_155):
            case char(Globals::lwall_210):
            case char(Globals::lwall_183):
            case char(Globals::lwall_214):
            case char(Globals::lwall_220):
            case char(Globals::lwall_221):
            case char(Globals::lwall_222):
            case char(Globals::lwall_223):
                print = p_map[r][c];
                Draw::SetColor(Globals::cWALLS2);
                break;

            //short walls
            case char(Globals::lwall_180) :
            case char(Globals::lwall_192) :
            case char(Globals::lwall_197) :
            case char(Globals::lwall_217) :
            case char(Globals::lwall_193) :
            case char(Globals::lwall_195) :
            case char(Globals::lwall_191) :
            case char(Globals::lwall_194) :
            case char(Globals::lwall_196) :
            case char(Globals::lwall_218) :
            case char(Globals::lwall_179) :
            case char(Globals::lwall_190) :
            case char(Globals::lwall_181) :
            case char(Globals::lwall_198) :
            case char(Globals::lwall_207) :
            case char(Globals::lwall_216) :
            case char(Globals::lwall_201) :
            case char(Globals::lwall_205) :
            case char(Globals::lwall_215) :
            case char(Globals::lwall_182) :
                print = p_map[r][c];
                Draw::SetColor(Globals::cWALLS2);
                break;

            //pellets             
            case '.':
            case (char)Globals::pellet:
                print = (char)Globals::pellet;
                Draw::SetColor(Globals::cPELLETS);
                break;

            // powerups
            case 'o':
            case (char)Globals::powerup:
                print = (char)Globals::powerup;
                Draw::SetColor(Globals::cPELLETS);
                break;

            case Globals::player_start:
                print = (char)Globals::pacman_left_open;
                Draw::SetColor(Globals::cPLAYER);
                break;

            case Globals::fruit: // fruit general
                print = GetFruitChar();
                Draw::SetColor(GetFruitColor());
                break;

            case char(Globals::fCherry): // fruit
            case char(Globals::fStrawberry): // fruit
            case char(Globals::fApple): // fruit
            case char(Globals::fPear): // fruit
                print = p_map[r][c];
                Draw::SetColor(GetFruitColor(p_map[r][c]));
                break;

            case Globals::space: // ' ' = empty space
                print = p_map[r][c];
                Draw::SetColor(Globals::cWHITE); // black on black = not visible
                break;

            case Globals::one_way: // T = portal
                print = p_map[r][c];
                Draw::SetColor(Globals::cIMMOVABLE); // black on black = not visible
                break;

            case Globals::ghost_spawn_target:
            case char(Globals::corner_marker): // '+'
            case Globals::invisible_wall: // % = invisible wall
            case Globals::teleport: // $ = one way door for ghost spawn area
                print = p_map[r][c];
                Draw::SetColor(Globals::cEDITABLE); // black on black = not visible
                break;
            
            case Globals::pink_ghost: // blue ghost
                print = (char)Globals::ghost_down;
                Draw::SetColor(Globals::cPINK);
                break; 
            
            case Globals::yellow_ghost: // yellow ghost
                print = (char)Globals::ghost_down;
                Draw::SetColor(Globals::cYELLOW);
                break;
            
            case Globals::blue_ghost: // green ghost
                print = (char)Globals::ghost_down;
                Draw::SetColor(Globals::cBLUE);
                break;
            
            case Globals::red_ghost: // red ghost
                print = (char)Globals::ghost_down;
                Draw::SetColor(Globals::cRED);
                break;
            
            case char(Globals::cursor): // cursor
                print = (char)Globals::cursor;
                if (!p_editor->p_cursor->IsEditing()) { // flashing effect - signals edible sate of ghost
                    p_editor->p_cursor->Blink() ? Draw::SetColor(Globals::c_blackwhite) : Draw::SetColor(Globals::c_whiteblack);
                    p_editor->p_cursor->SetBlink();
                }
                else { // solid color
                    Draw::SetColor(Globals::c_bluewhite);
                }
                break;

            default:
                print = p_map[r][c];
                Draw::SetColor(Globals::cWALLS); // gray bg on gray text for all other chars making them walls essentially
                break;
            }

            // print char
            cout << print;

            // set color back to default
            Draw::SetColor(Globals::cWHITE);

            // write a space to delete resize left over
            cout << (c == cols - 1 ? " " : "");
        }
        // end of row ad line feed
        cout << endl;
        
    }

    ShowKey();

} 

int Scene::AddRemoveColumns(bool add)
{
     // set max and min bouderies for size of the level
    if ((!add && cols - 1 < Globals::min_scene_cols) || (add && cols + 1 > Globals::max_scene_cols))
    {
        // exit add/remove
        return 0;
    }

    // determine where to add/remove from
    bool left = cols_from_left;
    int new_cols = add ? cols + 1 : cols - 1;

    // create temp array with the new scene size
    char** p_map_new = new char* [rows];
    for (int i = 0; i < rows; i++)
    {
        p_map_new[i] = new char[new_cols];
    }

    // place pellets in the entire array
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < new_cols; col++)
        {
            p_map_new[row][col] = char(Globals::pellet);
        }
    }

    // insert from current array into new one
    int iCols = add ? cols : cols - 1;
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < iCols; col++)
        {
            if (add)
            {
                
                if (new_cols > 2) // fixes potential error warning on dynamic array
                {
                    p_map_new[row][left ? col : col + 1] = p_map[row][col];
                }
            }
            else
            {
                if (col + 1 <= new_cols) // fixes potential error warning on dynamic array
                {
                    p_map_new[row][col] = p_map[row][left ? col : col + 1];
                }
            }
        }
    }

    // shift rows
    if (add)
    {
        // shift the walls left or right and set pellets in preior state
        for (int row = 0; row < rows; row++)
        {
            row == NULL ? row = 0 : row = row; // this fixed it;
            if (new_cols > 2) // this fixed it;
            {
                p_map_new[row][left ? new_cols - 1 : 0] = p_map[row][left ? cols - 1 : 0];
                p_map_new[row][left ? new_cols - 2 : 1] = char(Globals::pellet);
            }
            
        } 

        rows == NULL ? rows = 0 : rows = rows; // this fixed it;
        if (new_cols > 2) // this fixed it;
        {
            p_map_new[0][left ? new_cols - 2 : 1] = char(Globals::invisible_wall);
            p_map_new[rows - 1][left ? new_cols - 2 : 1] = char(Globals::invisible_wall);
        }
    }
    else
    {
        // shift the walls left or right and set pellets in preior state
        for (int row = 0; row < rows; row++)
        {
            p_map_new[row][left ? new_cols - 1 : 0] = p_map[row][left ? cols - 1 : 0];
        }
    }
    
    // delete the current p_map
    DeallocateMapArray();

    // set the p_map to the new array
    p_map = p_map_new;
    cols = new_cols;

    // set the new aray to null
    p_map_new = nullptr;

    // set flag fromleft/right to opposite
    cols_from_left = !cols_from_left;

    //clean exit
    return 0;

}

int Scene::AddRemoveRows(bool add)
{
    // set max and min bouderies for size of the level
    if ((!add && rows - 1 < Globals::min_scene_rows) || (add && rows + 1 > Globals::max_scene_rows))
    {
        return 0;
    }
    
    // determine where to add/remove from
    bool left = rows_fom_bottom;
    int new_rows = add ? rows + 1 : rows - 1;

    // create temp array with the new scene size
    char** p_map_new = new char* [new_rows];
    for (int i = 0; i < new_rows; i++)
    {
        p_map_new[i] = new char[cols];
    }

    // place pellets in the entire new array
    for (int row = 0; row < new_rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            p_map_new[row][col] = char(Globals::pellet);
        }
    }

    // insert from current array into new one
    int iRows = add ? rows : rows - 1;
    for (int row = 0; row < iRows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            if (add)
            {
                if (new_rows > 2) // fxes potential dynamic array warning
                {
                    p_map_new[left ? row : row + 1][col] = p_map[row][col];
                }
            }
            else
            {
                if (new_rows > 2) // fxes potential dynamic array warning
                {
                    p_map_new[row][col] = p_map[left ? row : row + 1][col];
                }
            }
        }
    }


    // shift rows
    if (add)
    {
        // shift the walls left or right and set pellets in preior state
        for (int col = 0; col < cols; col++)
        {
            if (new_rows > 2) // fxes potential dynamic array warning
            {
                p_map_new[left ? new_rows - 1 : 0][col] = p_map[left ? rows - 1 : 0][col];
                p_map_new[left ? new_rows - 2 : 1][col] = char(Globals::pellet);
            }
        }

        if (new_rows > 2 && cols > 2) // fxes potential dynamic array warning
        {
            p_map_new[left ? new_rows - 2 : 1][0] = char(Globals::invisible_wall);
            p_map_new[left ? new_rows - 2 : 1][cols - 1] = char(Globals::invisible_wall);
        }
        
    }
    else
    {
        // shift the walls left or right and set pellets in preior state
        for (int col = 0; col < cols; col++)
        {
            p_map_new[left ? new_rows - 1 : 0][col] = p_map[left ? rows - 1 : 0][col];
        }
    }

    // place cursor on top left of console
    Draw::CursorTopLeft(rows + top_left_modifier); // + x for cpations

    // delete the current p_map
    DeallocateMapArray();

    // set the p_map to the new array
    p_map = p_map_new;
    rows = new_rows;

    // set the new aray to null
    p_map_new = nullptr;

    // set flag fromleft/right to opposite
    rows_fom_bottom = !rows_fom_bottom;

    // clean exit
    return 0;
}

void Scene::ResizeScene()
{

    if (add_cols)
    {
        AddRemoveColumns(true);
        add_cols = false;
    }

    if (add_rows)
    {
        AddRemoveRows(true);
        add_rows = false;
    }

    if (remove_cols)
    {
        AddRemoveColumns(false);
        remove_cols = false;
    }

    if (remove_rows)
    {
        AddRemoveRows(false);
        remove_rows = false;
    }

    // check cursor is in bounds
    if (p_editor->p_cursor->GetCurrentRow() > rows - 1) {
        p_editor->p_cursor->SetCurrentPosition(rows - 1, p_editor->p_cursor->GetCurrentCol());
    }
    if (p_editor->p_cursor->GetCurrentCol() > cols - 1) {
        p_editor->p_cursor->SetCurrentPosition(p_editor->p_cursor->GetCurrentRow(), cols - 1);
    }
}

// setters
void Scene::SetEditor(Editor* p_editor)
{
    this->p_editor = p_editor;
}

void Scene::ShowKey()
{
    int line_size = 31; string format = "";

    //erase previous content - up to 4 lines - and bring cursor back up
    for (int i = 0; i < 5; i++)
    {
        cout << Draw::WriteEmptyLine(line_size) << endl;
    }
    Draw::CursorTopLeft(6);
    
    // set cursor one line below scene
    cout << endl << endl;
    
    // display the right caption based on cursor state, errors, etc.
    if (error_count > 0)
    {
        cout << format << scene_errors;
        top_left_modifier = 5;
        // pause so use can read the error message
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        // reset error states
        scene_errors = "";
        error_count = 0;
    }
    else if (resize_scene)
    {
        cout << format << char(16) << char(17) << " adjust columns (" + to_string(cols) + ")" << endl;
        cout << format << char(30) << char(31) << " adjust rows (" + to_string(rows) + ")" << endl;
        top_left_modifier = 6;
    }
    else if (p_editor->p_cursor->IsEditing())
    {
        if (p_editor->p_cursor->Pen() == '0' && !resize_scene)
        {
            cout << format << "#|@|& = walls,  R = resize" << endl;
            cout << format << "T = teleport, ^ = spawn target" << endl;
            cout << format << ". = pellet, o = power, F = fruit" << endl;
            cout << format << "' ' = space, S = player" << endl;
            top_left_modifier = 8;
        }
        else if (p_editor->p_cursor->pen_is_walls || p_editor->p_cursor->pen_is_short_walls)
        {
            cout << format;
            Draw::SetColor(Globals::c_bluewhite);
            cout << p_editor->p_cursor->Pen();
            Draw::SetColor(Globals::cWHITE);
            cout << " active pen" << endl;
            cout << format << "-|= cycle shapes" << endl;
            cout << format << "Esc = swap pen" << endl;
            cout << format << "Space = disable" << endl;
            top_left_modifier = 8;
        }
        else if (p_editor->p_cursor->pen_is_fruits)
        {
            cout << format;
            Draw::SetColor(GetFruitColor(p_editor->p_cursor->Pen()));
            cout << p_editor->p_cursor->Pen();
            Draw::SetColor(Globals::cWHITE);
            cout << " active pen" << endl;
            cout << format << "-|= cycle shapes" << endl;
            cout << format << "Esc = swap pen" << endl;
            cout << format << "Space = disable" << endl;
            top_left_modifier = 8;
        }
        else
        {
            cout << format;
            Draw::SetColor(Globals::c_bluewhite);
            cout << p_editor->p_cursor->Pen();
            Draw::SetColor(Globals::cWHITE);
            cout << " active pen" << endl;
            cout << format << "Esc = swap pen" << endl;
            cout << format << "Space = disable" << endl;
            top_left_modifier = 7;
        }
        
    }
    else if (p_editor->IsDoneEditing())
    {
        cout << format << "Save? y = yes, n = no" << endl;
        cout << format << "Esc = cancel" << endl;
        top_left_modifier = 6;
    }
    else
    {
        cout << format << "Space = choose pen" << endl;
        top_left_modifier = 5;
    }
}

string** Scene::GetSceneValues()
{
    
    // if array of options already exists, delete it
    DeleteOptions();
    
    // create the options array
    p_options = new string * [12];
    for (int i = 0; i < 12; i++)
    {
        p_options[i] = new string[2];
    }
    p_options[0][0] = "0";
    p_options[0][1] = "Scene: " + to_string(this_scene);

    p_options[1][0] = "1";
    p_options[1][1] = "Title: " + title;

    p_options[2][0] = "2";
    p_options[2][1] = "Points p pellet: " + to_string(points_pellet);

    p_options[3][0] = "3";
    p_options[3][1] = "Points p ghots: " + to_string(points_ghost);

    p_options[4][0] = "4";
    p_options[4][1] = "Points all ghosts: " + to_string(all_ghost_bonus);

    p_options[5][0] = "9";
    p_options[5][1] = "Fruit points: " + to_string(fruit_points);

    p_options[6][0] = "5";
    p_options[6][1] = "Chase mode duration: " + to_string(chase_for);
    
    p_options[7][0] = "6";
    p_options[7][1] = "Run mode duration: " + to_string(run_for);
    
    p_options[8][0] = "7";
    p_options[8][1] = "Roam mode duration: " + to_string(roam_for);

    p_options[9][0] = "8";
    p_options[9][1] = "Roam count: " + to_string(roam_count);

    p_options[10][0] = "#save";
    p_options[10][1] = "SAVE";

    p_options[11][0] = "#cancel";
    p_options[11][1] = "CANCEL";

    return p_options;
 }

void Scene::DeleteOptions()
{
    if (p_options)
    {
        for (int i = 0; i < 10; i++)
        {

            delete[] p_options[i];
            p_options[i] = nullptr;
        }
        delete[] p_options;
        p_options = nullptr;

    }
}

bool Scene::PenIsValid(char pen)
{
    int count = 0;
    int max_count = 0;
    switch (pen)
    {
    case 'T':
        max_count = rows - 2; // cannot use corners for teleport
        break;
    case 'S':
    case '^':
        max_count = 1;
        break;
    default:
        return true;
    }

    // remove cursor and replace with current content of the cursor
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            if (p_map[r][c] == pen)
            {
                count++;
            }
        }
    }

    return (count > max_count ? false : true);
}

bool Scene::SaveToFile()
{
    string file_line; // will use to load in the line from the file
    string text_1 = ""; // will store text file lines until the point we need to insert
    string text_2 = ""; // will store the text file lines after the point of insertion
    string marker; // a string makrer for finding strings in strings
    bool is_part2 = false; // flag to let me know if we're in the second part of the text
    int scene_number = 0; // the scene number to use for the inserted scene
    
    ifstream read_scenes("PacMan.Scenes"); // open the scenes text file
    
    // get the text lines from file until point where we need to insert the update
    if (read_scenes)
    {
        // file exists and is open 
        while (getline(read_scenes, file_line))
        {
            marker = "#scene:";

            if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos)
            {
                scene_number++;
            }

            marker = "#scene:" + to_string(this_scene);

            if (file_line.find(Utility::TransformString(marker, 1), 0) == std::string::npos) {
                if (!is_part2) {
                    text_1 += file_line + "\n";
                }
                else {
                    text_2 += file_line + "\n";
                }
                continue;
            }
            else
            {
                is_part2 = true;
                
                if (this_scene == 0)
                {
                    // place the scene into text_2
                    text_2 += "\n" + file_line + "\n";
                    while (getline(read_scenes, file_line))
                    {
                        marker = "#end_scene";
                        if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos)
                        {
                            text_2 += file_line + "\n";
                            break; // skip
                        }
                        else
                        {
                            text_2 += file_line + "\n";
                        }
                    }
                }
                else
                {
                    // move past the scene
                    while (getline(read_scenes, file_line))
                    {
                        marker = "#end_scene";
                        if (file_line.find(Utility::TransformString(marker, 1), 0) == std::string::npos)
                        {
                            continue; // skip
                        }
                        else
                        {
                            break; // exit this loop
                        }
                    }

                }
                
                
            }

        }
        read_scenes.close();
    }

    // create a string with the edited scene contents
    string update = CreatesceneString(scene_number);

    // add in the first part of the file before
    // add the second part of the file after
    update = text_1 + update + text_2;
    
    // update the file in one shot overwriting whatever is there
    ofstream scene_file("PacMan.scenes");
    scene_file << update;

    // close the file
    scene_file.close();

    // return success
    return true;

}

string Scene::CreatesceneString(int scene)
{
    // create a string with the scene
    string map = "";

    // start by setting the boundry markers to '+' so we can decode the map size
    p_map[0][0] = char(Globals::corner_marker);
    p_map[0][cols - 1] = char(Globals::corner_marker);
    p_map[rows - 1][0] = char(Globals::corner_marker);
    p_map[rows - 1][cols - 1] = char(Globals::corner_marker);

    // add the map array content to the map string
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            //replace the pen character witht the right content
            switch (p_map[row][col])
            {
                case char(Globals::cursor) :
                    p_map[row][col] = p_editor->p_cursor->content_now;
                    break;
                case char(Globals::fCherry):
                    p_map[row][col] = Globals::fruit;
                    fruit = Fruits::CHERRY;
                    break;
                case char(Globals::fStrawberry) :
                    p_map[row][col] = Globals::fruit;
                    fruit = Fruits::STRAWBERRY;
                    break;
                case char(Globals::fApple) :
                    p_map[row][col] = Globals::fruit;
                    fruit = Fruits::APPLE;
                    break;
                case char(Globals::fPear) :
                    p_map[row][col] = Globals::fruit;
                    fruit = Fruits::PEAR;
                    break;
            }

            map += p_map[row][col];
        }
        map += "\n";
    }

    // replace the full stop with pellet char
    Utility::ReplaceString(map, char(Globals::pellet), '.');


    // set an update string with the scene variables that need to be saved
    string update = "";
    update += "#scene:" + to_string(this_scene != 0 ? this_scene : scene) + "\n";
    update += "title:" + title + "\n";
    update += "pellet_points:" + to_string(points_pellet) + "\n";
    update += "ghost_points:" + to_string(points_ghost) + "\n";
    update += "all_ghosts_bonus:" + to_string(all_ghost_bonus) + "\n";
    update += "chase_duration:" + to_string(chase_for) + "\n";
    update += "run_duration:" + to_string(run_for) + "\n";
    update += "roam_duration:" + to_string(roam_for) + "\n";
    update += "roam_count:" + to_string(roam_count) + "\n";
    update += "fruit:" + GetFruitString() + "\n";
    update += "fruit_points:" + to_string(fruit_points) + "\n";
    update += "level_map:\n";

    
    // combine the strings and add the end of scene marker
    update += map;
    update += "#end_scene\n";

    // return the scene update string
    return update;
}

bool Scene::ValiditeScene()
{
        
    int teleport = 0, player_start = 0, ghost_spawn_target = 0, scene_fruit = 0;
    bool no_dead_ends = true, has_outer_walls = true, all_pellets_reacheable, is_valid_scene = true, teleports_ok = true;
    scene_errors = "";

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            // do counts
            switch (p_map[row][col])
            {
                case char(Globals::teleport) :
                if (col > 0  && col < cols - 1)
                    teleports_ok = false;
                teleport++;
                break;
            case char(Globals::player_start):
                player_start++;
                break;
            case char(Globals::ghost_spawn_target):
                ghost_spawn_target++;
                break;
            case char(Globals::fCherry):
            case char(Globals::fStrawberry) :
            case char(Globals::fApple) :
            case char(Globals::fPear) :
                scene_fruit++;
                break;
            }

            // check for dead end
            if (!HasNoDeadEnd(row, col))
            {
                no_dead_ends = false;
            }

            // check for outer walls
            if (!HasOuterWalls(row, col))
            {
                has_outer_walls = false;
            }

        }
    }

     if (teleport % 2 != 0)
    {
        scene_errors = "You need pairs of teleports\n";
        is_valid_scene = false;
        error_count++;
        return false;
    }

     if (!teleports_ok)
     {
         scene_errors = "Place teleports on side walls\n";
         is_valid_scene = false;
         error_count++;
         return false;
     }

    if (player_start > 1 || player_start < 1)
    {
        scene_errors = "You need one player start position.\n";
        is_valid_scene = false;
        error_count++;
        return false;
    }

    if (ghost_spawn_target > 1 || ghost_spawn_target < 1)
    {
        scene_errors = "You need one ghosts spawn target.\n";
        is_valid_scene = false;
        error_count++;
        return false;
    }

    if (scene_fruit > 1 || scene_fruit < 1)
    {
        scene_errors = "You need one fruit spawn.\n";
        is_valid_scene = false;
        error_count++;
        return false;
    }

    if (!no_dead_ends)
    {
        scene_errors = "The level cannot have dead ends.\n";
        is_valid_scene = false;
        error_count++;
        return false;
    }

    if (!has_outer_walls)
    {
        scene_errors = "You need outer walls around the scene.\n";
        is_valid_scene = false;
        error_count++;
        return false;
    }

    if (!(all_pellets_reacheable = AlPelletsReacheable()))
    {
        scene_errors = "You have unreacheable pellets.\n";
        is_valid_scene = false;
        error_count++;
        return false;
    }

    return is_valid_scene;

}

bool Scene::HasOuterWalls(int row, int col)
{
    if (col == 0 || col == cols || row == 0 || row == rows)
    {
        switch (p_map[row][col])
        {
        //special walls
        case char(Globals::corner_marker):
        case '|':
        case '-':
        case '#':
        case '%':
        //long walls
        case char(Globals::lwall_184):
        case char(Globals::lwall_187):
        case char(Globals::lwall_188):
        case char(Globals::lwall_200):
        case char(Globals::lwall_203):
        case char(Globals::lwall_209):
        case char(Globals::lwall_212):
        case char(Globals::lwall_213):
        case char(Globals::lwall_155):
        case char(Globals::lwall_210):
        case char(Globals::lwall_183):
        case char(Globals::lwall_214):
        case char(Globals::lwall_220):
        case char(Globals::lwall_221):
        case char(Globals::lwall_222):
        case char(Globals::lwall_223):
        //short walls
        case char(Globals::lwall_180):
        case char(Globals::lwall_192):
        case char(Globals::lwall_197):
        case char(Globals::lwall_217):
        case char(Globals::lwall_193):
        case char(Globals::lwall_195):
        case char(Globals::lwall_191):
        case char(Globals::lwall_194):
        case char(Globals::lwall_196):
        case char(Globals::lwall_218):
        case char(Globals::lwall_179):
        case char(Globals::lwall_190):
        case char(Globals::lwall_181):
        case char(Globals::lwall_198):
        case char(Globals::lwall_207):
        case char(Globals::lwall_216):
        case char(Globals::lwall_201):
        case char(Globals::lwall_205):
        case char(Globals::lwall_215):
        case char(Globals::lwall_182):
            return true;
        }
    }
    return true;
}

bool Scene::HasNoDeadEnd(int row, int col)
{
    // replace the cursor with the content from the sqaure its on
    p_map[p_editor->p_cursor->GetCurrentRow()][p_editor->p_cursor->GetCurrentCol()] = p_editor->p_cursor->content_now;
    
    // if in baounds of the outer walls
    if (col > 0 && col - 1 < cols && row > 0 && row < rows - 1)
    {
        
        // and the current position is a pellet
        if (p_map[row][col] == char(Globals::pellet))
        {
            // look around the sqaure for other pellet sqaures
            int route_count = 0; int this_row = 0, this_col = 0;
            for (int i = 0; i < 4; i++)
            {

                this_row = row, this_col = col;
                switch (i)
                {
                case 0: // above
                    this_row = row - 1;
                    break;
                case 1: // to right
                    this_col = col + 1;
                    break;
                case 2: // below
                    this_row = row + 1;
                    break;
                case 3: // to left
                    this_col = col - 1;
                    break;
                }

                
                switch (p_map[this_row][this_col])
                {
                case char(Globals::space):
                case char(Globals::pellet):
                case char(Globals::powerup):
                case 'o':
                case char(Globals::player_start):
                case char(Globals::ghost_spawn_target):
                case char(Globals::fruit):
                case char(Globals::fCherry) :
                case char(Globals::fStrawberry) :
                case char(Globals::fApple) :
                case char(Globals::fPear) :
                    route_count++;
                    break;
                default:
                    break;
                }

            }

            // if the number of pellets around the sqaure is more than 1 there are no dead ends
            return route_count > 1 ? true : false;

        }

        return true;
    }

    return true;
}

bool Scene::AlPelletsReacheable()
{
    Coord player_start;
    bool is_match = false;
    bool pellets_reacheable = true;
    
    char** temp = nullptr;
    temp = new char* [rows];
    for (int i = 0; i < rows; i++)
    {
        temp[i] = new char[cols];
    }

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            temp[row][col] = p_map[row][col];
            
            if (p_map[row][col] == char(Globals::player_start))
            {
                player_start = { row, col };
            }

            if (p_map[row][col] == char(Globals::cursor))
            {
                temp[row][col] = p_editor->p_cursor->content_now;
            }
        }
    }

    Coord current_position = player_start;
    Coord new_position = current_position;

    for (int dir = 0; dir < 4; dir++)
    {
        // set the new position
        switch (static_cast<Direction>(dir))
        {
        case Direction::UP:
            if (new_position.row > 0)
                new_position = current_position + Direction::UP;
            break;
        case  Direction::RIGHT:
            if (new_position.col < cols - 1)
                new_position = current_position + Direction::RIGHT;
            break;
        case  Direction::DOWN:
            if (new_position.row < rows - 1)
                new_position = current_position + Direction::DOWN;
            break;
        case  Direction::LEFT:
            if (new_position.col > 0)
                new_position = current_position + Direction::LEFT;
            break;
        }

        // if the new position is the same then no place to go so continue
        if (new_position == current_position)
        {
            continue;
        }

        // if the new position is in boounds, ser reachable character
        if (new_position.row < rows && new_position.row >= 0 && new_position.col < cols && new_position.col >= 0)
        {
            switch (temp[new_position.row][new_position.col])
            {
            case char(Globals::pellet):
            case char(Globals::powerup):
            case 'o':
            case '.':
            case char(Globals::player_start):
            case char(Globals::ghost_spawn_target):
            case char(Globals::space):
            case char(Globals::fruit):
                is_match = true;
                break;
            default:
                is_match = false;
                break;
            }

            if (is_match)
            {
                temp[new_position.row][new_position.col] = 'A';
                SetPositionToAccessible(new_position, temp);
            }
        }

        // revert new positoin back to the intial posiotion to iterate
        new_position = current_position;

    }

    // search for pellets in the temp array
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            if (temp[row][col] == char(Globals::pellet))
            {
                // if one found, then set reacheable to false
                pellets_reacheable = false;
            }
        }
    }

    // clean up temp array
    for (int i = 0; i < rows; i++)
    {
        delete[] temp[i];
    }
    delete[] temp;
    temp = nullptr;

    // return reacheable
    return pellets_reacheable;
}

void Scene::SetPositionToAccessible(Coord new_position, char** temp)
{
    // save the orginal positon coming in
    Coord current_position = new_position;
    bool is_match = false;

    // iterate the possible directions
    for (int dir = 0; dir < 4; dir++)
    {
        // set the new position
        switch (static_cast<Direction>(dir))
        {
        case Direction::UP:
            if (new_position.row > 0)
                new_position = current_position + Direction::UP;
            break;
        case Direction::RIGHT:
            if (new_position.col < cols - 1)
                new_position = current_position + Direction::RIGHT;
            break;
        case  Direction::DOWN:
            if (new_position.row < rows - 1)
                new_position = current_position + Direction::DOWN;
            break;
        case  Direction::LEFT:
            if (new_position.col > 0)
                new_position = current_position + Direction::LEFT;
            break;
        }

        // if the new position is the same then no place to go so continue
        if (new_position == current_position)
        {
            continue;
        }

        // if the new position is in boounds, ser reachable character
        if (new_position.row < rows && new_position.row >= 0 && new_position.col < cols && new_position.col >= 0)
        {
            switch (temp[new_position.row][new_position.col])
            {
            case char(Globals::pellet) :
            case char(Globals::powerup) :
            case 'o':
            case '.':
            case char(Globals::player_start) :
            case char(Globals::ghost_spawn_target) :
            case char(Globals::space) :
            case char(Globals::fruit) :
            case char(Globals::fCherry) :
            case char(Globals::fStrawberry) :
            case char(Globals::fApple) :
            case char(Globals::fPear) :
                is_match = true;
                break;
            default:
                is_match = false;
                break;
            }

            if (is_match)
            {
                temp[new_position.row][new_position.col] = 'A';
                SetPositionToAccessible(new_position, temp);
            }
        }

        // revert new positoin back to the intial posiotion to iterate
        new_position = current_position;

    }
}

void Scene::DeallocateMapArray()
{
    // deallocated map array
    if (p_map)
    {
        for (int i = 0; i < rows; i++) {
            delete[] p_map[i];
        }
        delete[] p_map;
        p_map = nullptr;
    }

}

Fruits Scene::GetFruitType(const string fruit)
{
    if (Utility::TransformString(fruit, 1) == "cherry")
        return Fruits::CHERRY;

    if (Utility::TransformString(fruit, 1) == "strawberry")
        return Fruits::STRAWBERRY;

    if (Utility::TransformString(fruit, 1) == "apple")
        return Fruits::APPLE;

    if (Utility::TransformString(fruit, 1) == "pear")
        return Fruits::PEAR;

    return Fruits::NONE;
}

char Scene::GetFruitChar()
{
    if (fruit == Fruits::CHERRY)
        return char(Globals::fCherry);

    if (fruit == Fruits::STRAWBERRY)
        return char(Globals::fStrawberry);

    if(fruit == Fruits::APPLE)
        return char(Globals::fApple);

    if (fruit == Fruits::PEAR)
        return char(Globals::fPear);

    return char(Globals::fCherry);
}

string Scene::GetFruitString()
{
    if (fruit == Fruits::CHERRY)
        return "cherry";

    if (fruit == Fruits::STRAWBERRY)
        return "strawberry";

    if (fruit == Fruits::APPLE)
        return "apple";

    if(fruit == Fruits::PEAR)
        return "pear";

    return "cherry";
}

int Scene::GetFruitColor()
{
    if (fruit == Fruits::CHERRY)
        return char(Globals::cCHERRY);

    if (fruit == Fruits::STRAWBERRY)
        return char(Globals::cSTRAWBERRY);

    if (fruit == Fruits::APPLE)
        return char(Globals::cAPPLE);

    if (fruit == Fruits::PEAR)
        return char(Globals::cPEAR);

    return char(Globals::cCHERRY);
}

int Scene::GetFruitColor(const char fruit_pen)
{
    if (fruit_pen == char(Globals::fCherry))
        return char(Globals::cCHERRY);

    if (fruit_pen == char(Globals::fStrawberry))
        return char(Globals::cSTRAWBERRY);

    if (fruit_pen == char(Globals::fApple))
        return char(Globals::cAPPLE);

    if (fruit_pen == char(Globals::fPear))
        return char(Globals::cPEAR);

    return char(Globals::cCHERRY);
}