#include "GameController.h"
#include "InputHandler.h"
#include "Renderer.h"
#include "GameLogic.h"
#include "ResourceManager.h"
#include "RecordManager.h"
#include "Constants.h"

static void resetGameState(Game& game, bool resetScore) {
    game.clock.restart();
    game.moveTimer = 0.0f;
    game.started = false;
    game.startTimer = START_DELAY;
    game.paused = false;
    if (resetScore) {
        game.score = 0;
        game.newRecord = false;
        game.playerName = "Player";
    }
}

void startNewGame(Game& game) {
    initGame(game);
    game.state = Game::PLAYING;
    resetGameState(game, true);
    if (game.musicEnabled && game.music.getStatus() != sf::Music::Playing)
        game.music.play();
}

void resumeGame(Game& game) {
    game.paused = false;
    game.state = Game::PLAYING;
    resetGameState(game, false);
    if (game.musicEnabled)
        game.music.play();
}

void goToMenu(Game& game) {
    game.state = Game::MENU;
    if (game.musicEnabled)
        game.music.stop();
}

void runGame(Game& game) {
    while (game.window.isOpen()) {
        float dt = game.clock.restart().asSeconds();
        handleInput(game);

        switch (game.state) {
        case Game::PLAYING: {
            if (!game.started) {
                game.startTimer -= dt;
                if (game.startTimer <= 0.0f) {
                    game.started = true;
                    if (game.soundEnabled) game.soundStart.play();
                }
                renderGame(game);
                break;
            }
            if (!game.paused) {
                game.moveTimer += dt;
                while (game.moveTimer >= game.moveInterval) {
                    game.moveTimer -= game.moveInterval;
                    updateGame(game);
                    if (!game.snake.alive) break;
                }
            }
            renderGame(game);
            break;
        }
        case Game::PAUSED:    renderPause(game); break;
        case Game::GAMEOVER:  renderGameOver(game); break;
        case Game::ENTER_NAME: renderEnterName(game); break;
        case Game::MENU:      drawMenu(game); break;
        case Game::DIFFICULTY_SELECT: drawDifficultySelect(game); break;
        case Game::RECORDS_TABLE: drawRecordsTable(game); break;
        case Game::SETTINGS:  drawSettings(game); break;
        }
    }
}