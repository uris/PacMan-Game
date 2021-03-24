#pragma once
#include "Character.h"

class Player : public Character
{

private:
    bool is_super = false;
    int lives = 3;
    int score = 0;
    bool ghosts_eaten[4] = { false, false, false, false };
    char player_move_content = '.';

public:
    //constructors
    Player();

    // methods
    void MovePlayer(char map_contents[]);
    void AddLives(int number);
    void TakeLives(int number);
    void ReSpawn();
    void EatGhost(int ghost_index);
    void EatGhost(char character);
    void ClearEatenGohsts();
    bool HasNoLives();
    bool AllGhostsEaten();

    // getters
    char GetMovedIntoSquareContents();
    int Lives();
    void PowerUp(bool powerup);
    int GetScore();

    // setters
    void SetLives();
    void SetLives(int number);
    void SetScore(int score);
    void SetMovedIntoSquareContents(char ascii);

};
