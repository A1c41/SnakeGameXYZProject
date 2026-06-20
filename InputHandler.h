#pragma once
#include "Structures.h"

void handleInput(Game& game);
void handleMenuInput(Game& game, const sf::Event& event);
void handleDifficultyInput(Game& game, const sf::Event& event);
void handleRecordsInput(Game& game, const sf::Event& event);
void handleSettingsInput(Game& game, const sf::Event& event);
void handlePlayingInput(Game& game, const sf::Event& event);
void handlePauseInput(Game& game, const sf::Event& event);
void handleGameOverInput(Game& game, const sf::Event& event);
void handleEnterNameInput(Game& game, const sf::Event& event);
void handleEnterNameTextInput(Game& game, const sf::Event& event);