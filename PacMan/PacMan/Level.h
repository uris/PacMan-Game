#pragma once
#include <chrono>
#include <string>
#include "Stage.h"
#include "Coord.h"
#include "EnumsAndStatics.h"

using namespace std;
using namespace std::chrono;

class Level : public Stage
{
    class Game* p_game = nullptr; // fwd declare class

public:

    // level map and scene
    char** p_map = nullptr;
    Coord** p_teleport = nullptr;
    bool level_paused = true;

    // level stats
    int eaten_pellets = 0; // pellets consumed
    int all_eaten_ghosts = 0; // pellets consumed
    bool is_complete = false;

    // Manange modes
    // bool ghosts_in_a_row[4]{false,false,false,false};
    Mode level_mode = Mode::CHASE;
    high_resolution_clock::time_point roam_time_start = chrono::high_resolution_clock::now();
    high_resolution_clock::time_point chase_time_start = chrono::high_resolution_clock::now();
    high_resolution_clock::time_point run_time_start = chrono::high_resolution_clock::now();
    duration<double> roam_duration = duration_cast<duration<double>>(roam_time_start - chrono::high_resolution_clock::now());
    duration<double> chase_duration = duration_cast<duration<double>>(chase_time_start - chrono::high_resolution_clock::now());
    duration<double> run_duration = duration_cast<duration<double>>(run_time_start - chrono::high_resolution_clock::now());


	//constructors
	Level();

    //constructors
    ~Level();

	// methods
    int SetupLevel(int& current_scene);
    int CreateLevelScene(int& current_scene);
    string LoadSceneFromFile(string filename, int scene_to_load);
    Coord MapSize(const string& map);
    void DrawLevel();
    bool NotWall(const Coord& move, const Direction& direction);
    bool CheckLevelComplete();
    bool IsTeleport(const Coord& coord);
    Ghosts IsGhost(const char& map_char);
    Fruits GetFruitType(const string fruit);
    void ResetLevel();

    // getters
    bool GameRefIsSet();

    //setter for forward class
    void SetGameRef(Game* p_game);
};