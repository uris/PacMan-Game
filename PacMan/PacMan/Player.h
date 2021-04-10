#pragma once
#include "Character.h"
//#include "Ghost.h"

//class IPlayerInterface : public Character
//{
//    virtual Coord GetCurrentPosition() {};
//};

class Player : public Character
{
    class Game* p_game = nullptr; // fwd declare class

private:
    bool is_super = false;
    int lives = 3;
    int score = 0;
    bool ghosts_eaten[4] = { false, false, false, false };
    char player_move_content = '.';
    bool chomp = false;

public:
    //constructors
    Player();

    // methods
    void MovePlayer();
    void AddLives(int number);
    void TakeLives(int number);
    void ReSpawn();
    void EatGhost(int ghost_index);
    void EatGhost(char character);
    void ClearEatenGohsts();
    bool HasNoLives();
    bool AllGhostsEaten();
    bool PayerGhostCollision(int ghost_index);
    void CoutPlayer();

    // getters
    char GetMovedIntoSquareContents();
    int Lives();
    void PowerUp(bool powerup);
    int GetScore();
    Coord GetCurrentPosition();
    bool GameRefIsSet();

    // setters
    void SetLives();
    void SetLives(int number);
    void SetScore(int score);
    void SetMovedIntoSquareContents(char ascii);
    void SetGameRef(Game* p_game);
};