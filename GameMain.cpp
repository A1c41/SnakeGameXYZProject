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

    // Загрузка ресурсов
    if (!loadResources(game)) {
        // Можно вывести сообщение, но игра продолжит работать с запасными текстурами
    }

    loadRecords(game);

    // Начальное состояние
    game.state = Game::MENU;
    game.difficulty = 1;
    game.scorePerApple = 2;
    game.menuSelection = 0;
    game.settingsSelection = 0;
    game.difficultySelection = 0;
    game.gameOverChoice = 0;
    game.pauseChoice = 0;

    // Запуск главного цикла
    runGame(game);

    return 0;
}