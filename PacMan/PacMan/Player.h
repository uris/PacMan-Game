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
    int lives = 3;
    int score = 0;
    bool ghosts_eaten[4] = { false, false, false, false };
    int fruits_eaten[4] = { 0, 0, 0, 0 };
    char player_move_content = '.';
    bool chomp = false;
    bool die_animation = false;
    bool eat_ghost_animation = false;
    bool player_flash = false;

public:
    
    //constructors
    Player();

    // destructor
    ~Player();

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
    bool PlayerGhostCollision(int ghost_index);
    bool PlayerFruitCollision();
    void CoutPlayer();
    void DeathAnimate(int g);
    void EatGhostAnimate(int g, bool life);
    void IncrementScore(const Object object_eaten);
    void UpdateMapAfterMove();

    // getters
    char GetMovedIntoSquareContents();
    int Lives();
    int GetScore();
    Coord GetCurrentPosition();
    bool GameRefIsSet();
    bool GetEatGhostAnimate();
    bool GetDieAnimate();
    void CoutEatenFruits();

    // setters
    void SetLives();
    void SetLives(int number);
    void SetGameRef(Game* p_game);
    void SetDeathAnimation(const bool die_animation);
    void SetEatGhostAnimate(const bool eat_ghost_animation);
    void AddEatenFruit(const Fruits fruit);
};