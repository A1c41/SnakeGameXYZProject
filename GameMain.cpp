#include "Functions.h"
#include "Constants.h"
#include <cstdlib>
#include <ctime>

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    Game game;
    game.window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake Game");
    game.window.setFramerateLimit(60);

    if (!loadResources(game)) {}

    loadRecords(game);

    game.difficulty = 1;
    game.scorePerApple = 2;
    game.baseSpeed = 0.5f;
    game.menuSelection = 0;
    game.settingsSelection = 0;
    game.difficultySelection = 0;
    game.gameOverChoice = 0;
    game.pauseChoice = 0;
    game.state = Game::MENU;

    while (game.window.isOpen()) {
        handleInput(game);

        if (game.state == Game::PLAYING) {
            float dt = game.clock.restart().asSeconds();

            if (!game.started) {
                game.startTimer -= dt;
                if (game.startTimer <= 0.0f) {
                    game.started = true;
                    if (game.soundEnabled) game.soundStart.play();
                }
                renderGame(game);
                continue;
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
        }
        else if (game.state == Game::PAUSED) {
            renderPause(game);
        }
        else if (game.state == Game::GAMEOVER) {
            renderGameOver(game);
        }
        else if (game.state == Game::ENTER_NAME) {
            renderEnterName(game);
        }
        else if (game.state == Game::MENU) {
            drawMenu(game);
        }
        else if (game.state == Game::DIFFICULTY_SELECT) {
            drawDifficultySelect(game);
        }
        else if (game.state == Game::RECORDS_TABLE) {
            drawRecordsTable(game);
        }
        else if (game.state == Game::SETTINGS) {
            drawSettings(game);
        }
    }

    return 0;
}