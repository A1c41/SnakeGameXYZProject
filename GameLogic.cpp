#include "GameLogic.h"
#include "Constants.h"
#include "RecordManager.h"
#include <cstdlib>

struct DifficultyLevel {
    float interval;
    int scorePerApple;
};

static const DifficultyLevel DIFFICULTY_TABLE[] = {
    {0.5f, 2}, {0.4f, 4}, {0.3f, 6}, {0.22f, 8}, {0.15f, 10}
};

sf::Vector2i getRandomFreePosition(const Game& game) {
    for (int attempts = 0; attempts < 1000; ++attempts) {
        int x = 1 + rand() % (GRID_WIDTH - 2);
        int y = 1 + rand() % (GRID_HEIGHT - 2);
        sf::Vector2i pos(x, y);
        bool free = true;
        for (auto& seg : game.snake.body) {
            if (seg == pos) { free = false; break; }
        }
        if (free && pos != game.food.position)
            return pos;
    }
    return sf::Vector2i(1, 1);
}

void initGame(Game& game) {
    game.snake.body.clear();
    int startX = GRID_WIDTH / 2;
    int startY = GRID_HEIGHT / 2;
    game.snake.body.push_back(sf::Vector2i(startX, startY));
    game.snake.body.push_back(sf::Vector2i(startX - 1, startY));
    game.snake.body.push_back(sf::Vector2i(startX - 2, startY));
    game.snake.direction = sf::Vector2i(1, 0);
    game.snake.nextDirection = sf::Vector2i(1, 0);
    game.snake.alive = true;

    int idx = std::min(std::max(game.difficulty - 1, 0), 4);
    game.moveInterval = DIFFICULTY_TABLE[idx].interval;
    game.scorePerApple = DIFFICULTY_TABLE[idx].scorePerApple;

    game.food.position = getRandomFreePosition(game);
    game.food.sprite = sf::Sprite(game.texFood);
    game.food.sprite.setPosition(
        static_cast<float>(game.food.position.x * CELL_SIZE),
        static_cast<float>(game.food.position.y * CELL_SIZE)
    );

    game.moveTimer = 0.0f;
    game.started = false;
    game.startTimer = START_DELAY;
    game.paused = false;
    game.score = 0;
    game.newRecord = false;
    game.playerName = "Player";
}

void spawnFood(Game& game) {
    game.food.position = getRandomFreePosition(game);
    game.food.sprite.setPosition(
        static_cast<float>(game.food.position.x * CELL_SIZE),
        static_cast<float>(game.food.position.y * CELL_SIZE)
    );
}

void checkCollisions(Game& game) {
    for (size_t i = 1; i < game.snake.body.size(); ++i) {
        if (game.snake.body[0] == game.snake.body[i]) {
            game.snake.alive = false;
            return;
        }
    }
    if (game.snake.body[0].x < 1 || game.snake.body[0].x >= GRID_WIDTH - 1 ||
        game.snake.body[0].y < 1 || game.snake.body[0].y >= GRID_HEIGHT - 1) {
        game.snake.alive = false;
    }
}

void updateGame(Game& game) {
    if (!game.snake.alive || game.paused || !game.started) return;

    game.snake.direction = game.snake.nextDirection;
    sf::Vector2i newHead = game.snake.body[0] + game.snake.direction;
    game.snake.body.push_front(newHead);

    bool ate = (newHead == game.food.position);
    if (ate) {
        game.score += game.scorePerApple;
        if (game.soundEnabled) game.soundEat.play();
        spawnFood(game);
    }
    else {
        game.snake.body.pop_back();
    }

    checkCollisions(game);

    if (!game.snake.alive) {
        if (game.soundEnabled) game.soundCrash.play();
        if (isHighScore(game, game.score)) {
            game.newRecord = true;
            game.state = Game::ENTER_NAME;
            game.playerName = "Player";
        }
        else {
            game.state = Game::GAMEOVER;
        }
    }
}