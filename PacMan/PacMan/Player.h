#pragma once
#include "Character.h"
#include "Ghost.h"

class Player : public Character
{

private:
    bool is_super = false;
    int lives = 3;
    int score = 0;
    bool ghosts_eaten[4] = { false, false, false, false };
    char player_move_content = '.';

public:
    // variables
    static constexpr char character = 67; // 'C'

    //constructors
    Player();

    // destructors
    ~Player();

    // methods
    void SetLives();
    void SetLives(int number);
    int Lives();
    void AddLives(int number);
    void TakeLives(int number);
    void PowerUp(bool powerup);
    void SetScore(int score);
    int GetScore();
    void ReSpawn();
    void EatGhost(int ghost_index);
    void EatGhost(char character);
    void ClearEatenGohsts();
    bool AllGhostsEaten();
    bool HasNoLives();
    void SetMovedIntoSquareContents(char ascii);
    char GetMovedIntoSquareContents();
    bool HasCollided(Ghost& ghost);
};
