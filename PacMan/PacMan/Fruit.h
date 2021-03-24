#pragma once
#include "Character.h"
#include "EnumsAndStatics.h"

class Fruit : public Character
{
private:

    // gets set once
    char fruit;
    int color = 71;
    int wait = 15; // time to wait between spawns
    int spawns = 0;
    int max_spawns = 2;
    Mode mode = Mode::ROAM;

    // gets set once based based on current level
    Coord roam_target; // roams to top right hand side
    Coord roam_points[4]{}; // will roam to 4 four spots and then exit if not eaten

    // changes as the game / level changes
    char square_content_now = ' ';
    char square_content_prior = ' ';

public:
    // constructors
    Fruit(char fruit);
    Fruit(char fruit, Coord roam_points[], int roam_points_size);

    // methods
    int DistanceToRoamTarget();
    bool PlayerCollision(Coord player_coord);
    void DecreaseWait();
    void MoveFruit(const  Coord player_coord, const Direction direction, const char map_content);

    // Getters
    Coord GetRoamTarget();
    char GetPreviousSqaureContent();
    char GetContentCurrent();
    int GetColor();
    int GetWait();
    bool SkipTurn();
    char FruitChar();

    // Setters
    void SetPreviousSqaureContent(char content);
    void SetContentCurrent(char content);
    void SetContentCurrent(bool same);
    void SetWait(int wait);
    void SpawnFruit();
};