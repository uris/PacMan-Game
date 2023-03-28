#include <fstream> // file manipulation
#include <iostream> // print to console
#include <stdlib.h> // exit
#include <conio.h> // used to get input without requiring return
#include "Game.h"
#include "Draw.h"

using namespace std;
using namespace std::chrono;

//constructors
Level::Level()
{
	// do nothing for now
}

//destructors
Level::~Level()
{
    // deallocate the dynamic level array
    if (p_map)
    {
        for (int i = 0; i < rows; i++) {
            delete[] p_map[i];
        }
        delete[] p_map;
        p_map = nullptr;
    }

    // deallocate the dynamic teleport array
    if (p_teleports)
    {
        for (int i = 0; i < number_teleports; i++) {
            delete[] p_teleports[i];
        }
        delete[] p_teleports;
        p_teleports = nullptr;
    }

    if (p_game)
    {
        p_game = nullptr;
    }
}

// Public Methods

int Level::SetupLevel(int& current_scene)
{
    // create the scene map and set up player + ghosts accordingly
    int check = CreateLevelScene(current_scene);

    // if level is loaded get started
    if (check == 1)
    {
        // set the fruit type
        p_game->p_fruit->SetFruitType(fruit);

        // start the chase timer
        chase_time_start = chrono::high_resolution_clock::now();
    }
    
    // return the value of check
    return check;
}

int Level::CreateLevelScene(int& current_scene)
{
    // load level info and map from scenes file using games current level
    string map = LoadSceneFromFile("PacMan.scenes", current_scene);

    // if the string return "false" then you've reached the end of the game
    if (map == "false") {
        // there is no more level and you have reachd the end of the game
        return -1;
    }

    // parse through string to replace pellt and powerup markers to their ascii code
    Utility::ReplaceString(map, ".", char(Globals::pellet));
    Utility::ReplaceString(map, "o", char(Globals::powerup));

    // create dynamic two dimension pointer array to hold map
    Coord size = MapSize(map); // get width and height of the map
    rows = size.row; // set the map size variables for the level
    cols = size.col; // set the map size variables for the level

    char** p_mapArray = new char* [size.row];
    for (int i = 0; i < size.row; i++)
    {
        p_mapArray[i] = new char[size.col];
    }

    // create the dynamic array of teleports in the scene (will push tepeports into this)
    Coord** p_level_tps = nullptr;
    int teleports = NumberTeleports(map);
    int tp_index = 0;
    if (teleports > 0)
    {
        number_teleports = teleports;
        p_level_tps = new Coord * [teleports];
        for (int i = 0; i < teleports; i++)
        {
            p_level_tps[i] = new Coord[2];
        }
    }

    string::size_type map_index = 0;
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            // add to dynamic map array (will override certain marker chars so do this first)
            p_mapArray[row][col] = map[map_index];
            
            // count pellets if map index is pellet or powerup
            if (map[map_index] == (char)Globals::pellet || map[map_index] == (char)Globals::powerup)
                total_pellets++;
            
            // Set player spawn if map char index is 'S'
            if (map[map_index] == Globals::player_start)
                p_game->p_player->SetPositions(row, col);

            // Set fruit spawn if map char index 'F'
            if (map[map_index] == Globals::fruit)
            {
                p_game->p_fruit->SetPositions(row, col);
                fruit_spawn = { row, col };
                p_mapArray[row][col] = (char)Globals::space;
            }

            // Set ghost spawn if map char is '^'
            if (map[map_index] == Globals::ghost_spawn_target)
            {
                ghost_spawn = { row, col };
                p_mapArray[row][col] = (char)Globals::space;

                // set ghosts spawn target
                for (int g = 0; g < Globals::total_ghosts; g++) // loop through ghosts
                {
                    p_game->p_ghosts[g]->SetSpawnTarget(row, col);
                }
            }

            // set ghost start spawn if map char is R,Y,B,P
            if (IsGhost(map[map_index]) != Ghosts::NONE)
            {
                p_game->p_ghosts[(int)IsGhost(map[map_index])]->SetPositions(row, col);
                p_mapArray[row][col] = (char)Globals::space;
            }

            // Set teleport if map char is T
            /*if (map[map_index] == Globals::teleport) {
                col == 0 ? tp_1 = { row, col } : tp_2 = { row, col };
            }*/
            if (map[map_index] == Globals::teleport && p_level_tps) {

                // check to make sure the tp index is in boundas of the tp array
                if (tp_index >= 0 && tp_index < teleports)
                {
                    if (col == 0) // the first teleport pad
                    {
                        p_level_tps[tp_index][0] = { row, col };
                    }
                    else if (col = cols -1) // the corresponding second second
                    {
                        p_level_tps[tp_index][1] = { row, col };
                        tp_index++;
                    }
                }
            }

            // increment the map index
            map_index++;
        }
    }

    // set ghost roam targets to edges of the level map
    p_game->p_ghosts[0]->SetRoamTarget({ rows + 3, cols - 3 });
    p_game->p_ghosts[1]->SetRoamTarget({ rows + 3, 3 });
    p_game->p_ghosts[2]->SetRoamTarget({ -3, 3 });
    p_game->p_ghosts[3]->SetRoamTarget({ -3 , cols - 3 });

    // push map array and telerpot arrays to their level pointer
    p_map = p_mapArray;
    p_teleports = p_level_tps;

    // set pointers to null (no delete!)
    p_mapArray = nullptr;
    p_level_tps = nullptr;

    // clean
    return 1;
}

Ghosts Level::IsGhost(const char& map_char)
{
    switch (map_char)
    {
        case char(Globals::red_ghost) :
            return Ghosts::RED;
            break;
        case char(Globals::yellow_ghost) :
            return Ghosts::YELLOW;
            break;
        case char(Globals::blue_ghost) :
            return Ghosts::BLUE;
            break;
        case char(Globals::pink_ghost) :
            return Ghosts::PINK;
            break;
    }
    return Ghosts::NONE;
}

string Level::LoadSceneFromFile(string filename, int scene_to_load)
{
    Utility utility; // access to utilities
    
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

Coord Level::MapSize(const string& map)
{
    size_t rows = 0, cols = 0;
    size_t pos;

    pos = map.find("+");
    if (pos != string::npos)
    {
        pos = map.find("+", pos + 1);
        if (pos != string::npos)
        {
            cols = pos + 1;
            pos = map.find("+", pos + 1);
            if (pos != string::npos) {
                rows = (pos / cols) + 1;
            }
        }
    }
    return { (int)rows, (int)cols };
}

int Level::NumberTeleports(const string& map)
{
    size_t teleports = 0;
    size_t start_pos = 0;

    while ((start_pos = map.find(Globals::teleport, start_pos)) != std::string::npos) {
        teleports++;
        start_pos += 1;
    }

    float tps = teleports / 2.0f;
    if ((int)tps == tps)
    {
        return (int)tps;
    }

    return -1;
}

void Level::DrawLevel()
{
    // set console size to accomodayte changes in rows/cols
    Draw::SetConsoleSize(p_game->GetResolution(), rows + 7, cols + 2);
    
    // remove cursor from screen to avoid the flicker
    Draw::ShowConsoleCursor(false);

    // place cursor on top left of console
    Draw::CursorTopLeft(rows + 20); // + 5 for title and status

    // Level Title
    string format = "";
    Draw::SetColor(Globals::cWHITE);
    cout << endl;

    Draw::SetColor(Globals::cPELLETS);
    format = Utility::Spacer("[{}", cols);
    cout << format;
    cout << "[{} ";
    cout << format;
    
    cout << endl;

    Draw::SetColor(7);
    format = Utility::Spacer(Utility::TransformString(title, 0), cols);
    cout << format;
    cout << Utility::TransformString(title, 0);
    cout << format;

    cout << endl << endl;

    // draw current level map
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {

            // ! - ordrer is important so player is on top always, followed by ghosts followed by fruit
            
            // position player
            if (p_game->p_player->GetCurrentPosition() == Coord(r, c))
            {
                p_game->p_player->CoutPlayer();
                continue;
            }

            // if ghosts in position, cout the ghost - R
            if (p_game->p_ghosts[0]->GetCurrentPosition() == Coord(r, c))
            {
                p_game->p_ghosts[0]->CoutGhost();
                continue;
            }

            // if ghosts in position, cout the ghost - Y
            if (p_game->p_ghosts[1]->GetCurrentPosition() == Coord(r, c))
            {
                p_game->p_ghosts[1]->CoutGhost();
                continue;
            }

            // if ghosts in position, cout the ghost - B
            if (p_game->p_ghosts[2]->GetCurrentPosition() == Coord(r, c))
            {
                p_game->p_ghosts[2]->CoutGhost();
                continue;
            }

            // if ghosts in position, cout the ghost - P
            if (p_game->p_ghosts[3]->GetCurrentPosition() == Coord(r, c))
            {
                p_game->p_ghosts[3]->CoutGhost();
                continue;
            }

            // if fruit in position, cout the fruit
            if (p_game->p_fruit->FruitActive() && p_game->p_fruit->GetCurrentPosition() == Coord(r, c))
            {
                p_game->p_fruit->CoutFruit();
                continue;
            }

            // set color of map content at this position
            switch (p_map[r][c])
            {

            //long walls
            case char(Globals::lwall_184) :
            case char(Globals::lwall_187) :
            case char(Globals::lwall_188) :
            case char(Globals::lwall_200) :
            case char(Globals::lwall_203) :
            case char(Globals::lwall_209) :
            case char(Globals::lwall_212) :
            case char(Globals::lwall_213) :
            case char(Globals::lwall_155) :
            case char(Globals::lwall_210) :
            case char(Globals::lwall_183) :
            case char(Globals::lwall_214) :
            case char(Globals::lwall_220) :
            case char(Globals::lwall_221) :
            case char(Globals::lwall_222) :
            case char(Globals::lwall_223) :
                Draw::SetColor(Globals::cWALLS2);
                cout << p_map[r][c];
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
                Draw::SetColor(Globals::cWALLS2);
                cout << p_map[r][c];
                break;

            //pellets             
            case '.':
            case (char)Globals::pellet:
                Draw::SetColor(Globals::cPELLETS);
                cout << (char)Globals::pellet;
                break;

            // powerups
            case 'o':
            case (char)Globals::powerup:
                Draw::SetColor(Globals::cPELLETS);
                cout << (char)Globals::powerup;
                break;

            case Globals::invisible_wall: // % = invisible wall
            case Globals::corner_marker: // + marker
            case Globals::teleport: // T = portal
            case Globals::space: // ' ' = empty space
            case Globals::one_way: // $ = one way door for ghost spawn area
                Draw::SetColor(Globals::cINVISIBLE); // black on black = not visible
                cout << p_map[r][c];
                break;

            default:
                Draw::SetColor(Globals::cWALLS); // gray bg on gray text for all other chars making them walls essentially
                cout << p_map[r][c];
                break;
            }

            // set color back to default
            Draw::SetColor(Globals::cWHITE);
        }
        // end of row ad line feed
        cout << endl;
    }

}

bool Level::NotWall(const Coord& move, const Direction& direction)
{
    switch (p_map[move.row][move.col])
    {
    case Globals::space:
    case Globals::teleport:
    case (char)Globals::pellet:
    case (char)Globals::powerup:
    case Globals::red_ghost:
    case Globals::yellow_ghost:
    case Globals::blue_ghost:
    case Globals::pink_ghost:
    case Globals::player:
    case Globals::fruit:
        return true;
        break;
    case Globals::one_way: // one way move to exit ghost spawn area
        return (direction != Direction::DOWN ? true : false);
        break;
    }
    return false;
}

bool Level::CheckLevelComplete()
{
    eaten_pellets >= total_pellets ? is_complete = true : is_complete = false; // if all pellets are eaten game ends - 339 in this map
    return is_complete;
}

bool Level::IsTeleport(const Coord& coord)
{
    if (tp_1 == coord || tp_2 == coord)
        return true;

    return false;
}

// getters
bool Level::GameRefIsSet()
{
    return (p_game ? true : false);
}

// setters
void Level::SetGameRef(Game* p_game)
{
    this->p_game = p_game;
}

Fruits Level::GetFruitType(const string fruit)
{
    if (Utility::TransformString(fruit, 1) == "cherry")
        return Fruits::CHERRY;

    if (Utility::TransformString(fruit,1) == "strawberry")
        return Fruits::STRAWBERRY;

    if (Utility::TransformString(fruit,1) == "apple")
        return Fruits::APPLE;

    if (Utility::TransformString(fruit,1) == "pear")
        return Fruits::PEAR;

    return Fruits::NONE;
}

void Level::ResetLevel()
{
    string title = "";
    int rows = 0;
    int cols = 0;
    level_mode = Mode::CHASE;
    roam_time_start = chrono::high_resolution_clock::now();
    chase_time_start = chrono::high_resolution_clock::now();
    run_time_start = chrono::high_resolution_clock::now();
    eaten_pellets = 0; // pellets consumed
    all_eaten_ghosts = 0; // pellets consumed
    is_complete = false;
    level_paused = true;
    total_pellets = 0;
    Fruits fruit = Fruits::NONE;
}