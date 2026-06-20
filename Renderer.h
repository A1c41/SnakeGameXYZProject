#pragma once
#include "Structures.h"

void drawBackgroundAndWalls(Game& game);
void drawRecordsList(Game& game, int maxCount, int startY);
void renderGame(Game& game);
void renderPause(Game& game);
void renderGameOver(Game& game);
void renderEnterName(Game& game);
void drawMenu(Game& game);
void drawDifficultySelect(Game& game);
void drawRecordsTable(Game& game);
void drawSettings(Game& game);