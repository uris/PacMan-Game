#pragma once
#include <string>
#include "Coord.h"
#include "Fruit.h"

using namespace std;

class Stage
{

public:
    // level map and scene
    string title = "Scene 1";
    int rows = 23;
    int cols = 47;
    Coord ghost_spawn;
    Coord fruit_spawn;
    Fruits fruit = Fruits::NONE;

    // teleport coords and direction
    Coord tp_1, tp_2;
    bool tp_row = true;
 
    // level stats
    int total_pellets = 0; // totala level pellets
    int points_pellet = 10;
    int points_ghost = 250;
    int all_ghost_bonus = 2600;
    int fruit_points = 0; // points for consuming the fruit

    // Manange modes
    int roam_count = 8;
    int roam_for = 6; // seconds
    int chase_for = 16; // seconds
    int run_for = 15; // seconds


    //constructors
    Stage() {};

    //constructors
    ~Stage() {};

};