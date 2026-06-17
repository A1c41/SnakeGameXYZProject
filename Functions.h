#pragma once
#include "Constants.h"
#include "Structures.h"

bool loadResources(Game& game);
void loadRecords(Game& game);
void saveRecords(const Game& game);
bool isHighScore(const Game& game, int score);
void updateRecords(Game& game);

sf::Texture createFallbackTexture(sf::Color color, int size = CELL_SIZE);
sf::Vector2i getRandomFreePosition(const Game& game);
void initGame(Game& game);
void spawnFood(Game& game);
void checkCollisions(Game& game);
void updateGame(Game& game);

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

void handleInput(Game& game);
void handleMenuInput(Game& game, const sf::Event& event);
void handleDifficultyInput(Game& game, const sf::Event& event);
void handleRecordsInput(Game& game, const sf::Event& event);
void handleSettingsInput(Game& game, const sf::Event& event);
void handlePlayingInput(Game& game, const sf::Event& event);
void handleGameOverInput(Game& game, const sf::Event& event);
void handleEnterNameInput(Game& game, const sf::Event& event);
void handlePauseInput(Game& game, const sf::Event& event);
void handleEnterNameTextInput(Game& game, const sf::Event& event);