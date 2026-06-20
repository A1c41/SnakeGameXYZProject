#include "GameController.h"
#include "InputHandler.h"
#include "Renderer.h"
#include "GameLogic.h"
#include "ResourceManager.h"
#include "RecordManager.h"
#include "Constants.h"

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
                    if (!game.snake.alive) {
                        break;
                    }
                }
            }
            renderGame(game);
            break;
        }
        case Game::PAUSED:
            renderPause(game);
            break;
        case Game::GAMEOVER:
            renderGameOver(game);
            break;
        case Game::ENTER_NAME:
            renderEnterName(game);
            break;
        case Game::MENU:
            drawMenu(game);
            break;
        case Game::DIFFICULTY_SELECT:
            drawDifficultySelect(game);
            break;
        case Game::RECORDS_TABLE:
            drawRecordsTable(game);
            break;
        case Game::SETTINGS:
            drawSettings(game);
            break;
        }
    }
}