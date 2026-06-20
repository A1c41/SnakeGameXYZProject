#pragma once
#include "Structures.h"

sf::Vector2i getRandomFreePosition(const Game& game);
void initGame(Game& game);
void spawnFood(Game& game);
void checkCollisions(Game& game);
void updateGame(Game& game);