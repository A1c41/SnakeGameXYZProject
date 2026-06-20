#include "GameController.h"
#include "ResourceManager.h"
#include "RecordManager.h"
#include "Constants.h"
#include <cstdlib>
#include <ctime>

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    Game game;
    game.window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake Game");
    game.window.setFramerateLimit(60);

    loadResources(game);
    loadRecords(game);

    game.state = Game::MENU;
    game.difficulty = 1;
    game.scorePerApple = 2;
    game.menuSelection = 0;
    game.settingsSelection = 0;
    game.difficultySelection = 0;
    game.gameOverChoice = 0;
    game.pauseChoice = 0;

    runGame(game);

    return 0;
}