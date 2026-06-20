#pragma once
#include "Structures.h"

void loadRecords(Game& game);
void saveRecords(const Game& game);
bool isHighScore(const Game& game, int score);
void updateRecords(Game& game);