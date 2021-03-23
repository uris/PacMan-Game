#include "Player.h"

Player::Player() {};

// destructors
Player::~Player() {};

// methods
void Player::SetLives()
{
	lives = lives;
}

void Player::SetLives(int number)
{
	lives = number;
}

int Player::Lives()
{
	return(lives);
}

void Player::AddLives(int number)
{
	lives += number;
}

void Player::TakeLives(int number)
{
	lives -= number;
}

void Player::PowerUp(bool powerup)
{
	is_super = powerup;
}

void Player::SetScore(int score)
{
	this->score = score;
}

int Player::GetScore()
{
	return(score);
}

void Player::ReSpawn()
{
	//previous_position = current_position;
	current_position = spawn_position;
	bool is_super = false;
	Direction current_direction = Direction::LEFT;
	Direction previous_direction = Direction::LEFT;
}

void Player::EatGhost(int ghost_index)
{
	ghosts_eaten[ghost_index] = true;
}

void Player::EatGhost(char character)
{
	switch (character)
	{
	case 'R':
		ghosts_eaten[0];
		break;
	case 'Y':
		ghosts_eaten[1];
		break;
	case 'B':
		ghosts_eaten[2];
		break;
	case 'P':
		ghosts_eaten[3];
		break;

	}
}

void Player::ClearEatenGohsts()
{
	for (int i = 0; i < sizeof(ghosts_eaten); i++)
	{
		ghosts_eaten[i] = false;
	}
}

bool Player::AllGhostsEaten()
{
	for (int i = 0; i < sizeof(ghosts_eaten); i++)
	{
		if (ghosts_eaten[i] == false)
			return false;
	}
	return true;
}

bool Player::HasNoLives()
{
	return(lives < 1 ? true : false);
}

void Player::SetMovedIntoSquareContents(char ascii)
{
	player_move_content = ascii;
}

char Player::GetMovedIntoSquareContents()
{
	return player_move_content;
}

bool Player::HasCollided(Ghost& ghost)
{
	return current_position.IsSame(ghost.GetCurrentPosition());
}