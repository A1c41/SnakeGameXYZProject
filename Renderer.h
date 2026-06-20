#pragma once
#include "Structures.h"

void drawGrid(sf::RenderWindow& window);
void drawWalls(Game& game);
void renderGame(Game& game);
void renderPause(Game& game);
void renderGameOver(Game& game);
void renderEnterName(Game& game);
void drawMenu(Game& game);
void drawDifficultySelect(Game& game);
void drawRecordsTable(Game& game);
void drawSettings(Game& game);