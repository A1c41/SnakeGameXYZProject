#include "Renderer.h"
#include "Constants.h"
#include <sstream>
#include <cmath>

void drawGrid(sf::RenderWindow& window) {
    for (int x = 0; x <= GRID_WIDTH; ++x) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(static_cast<float>(x * CELL_SIZE), 0.0f)),
            sf::Vertex(sf::Vector2f(static_cast<float>(x * CELL_SIZE), static_cast<float>(WINDOW_HEIGHT)))
        };
        window.draw(line, 2, sf::Lines);
    }
    for (int y = 0; y <= GRID_HEIGHT; ++y) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(0.0f, static_cast<float>(y * CELL_SIZE))),
            sf::Vertex(sf::Vector2f(static_cast<float>(WINDOW_WIDTH), static_cast<float>(y * CELL_SIZE)))
        };
        window.draw(line, 2, sf::Lines);
    }
}

void drawWalls(Game& game) {
    sf::Sprite wallSprite(game.texWall);
    for (int x = 0; x < GRID_WIDTH; ++x) {
        wallSprite.setPosition(static_cast<float>(x * CELL_SIZE), 0.0f);
        game.window.draw(wallSprite);
        wallSprite.setPosition(static_cast<float>(x * CELL_SIZE),
            static_cast<float>((GRID_HEIGHT - 1) * CELL_SIZE));
        game.window.draw(wallSprite);
    }
    for (int y = 1; y < GRID_HEIGHT - 1; ++y) {
        wallSprite.setPosition(0.0f, static_cast<float>(y * CELL_SIZE));
        game.window.draw(wallSprite);
        wallSprite.setPosition(static_cast<float>((GRID_WIDTH - 1) * CELL_SIZE),
            static_cast<float>(y * CELL_SIZE));
        game.window.draw(wallSprite);
    }
}

void renderGame(Game& game) {
    game.window.clear(sf::Color::Black);
    game.window.draw(game.backgroundSprite);
    drawWalls(game);

    game.window.draw(game.food.sprite);

    for (size_t i = 0; i < game.snake.body.size(); ++i) {
        sf::Sprite sprite;
        if (i == 0)
            sprite = sf::Sprite(game.texHead);
        else
            sprite = sf::Sprite(game.texBody);
        sprite.setPosition(static_cast<float>(game.snake.body[i].x * CELL_SIZE),
            static_cast<float>(game.snake.body[i].y * CELL_SIZE));
        game.window.draw(sprite);
    }

    std::stringstream ss;
    ss << "Score: " << game.score;
    sf::Text text(ss.str(), game.font, 18);
    text.setFillColor(sf::Color::Red);
    text.setPosition(10.0f, 10.0f);
    game.window.draw(text);

    if (!game.started && game.snake.alive) {
        int remaining = static_cast<int>(std::ceil(game.startTimer));
        sf::Text countdown(std::to_string(remaining), game.font, 48);
        countdown.setFillColor(sf::Color::Yellow);
        countdown.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 20),
            static_cast<float>(WINDOW_HEIGHT / 2 - 30));
        game.window.draw(countdown);
    }

    game.window.display();
}

void renderPause(Game& game) {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(WINDOW_WIDTH),
        static_cast<float>(WINDOW_HEIGHT)));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    game.window.draw(overlay);

    sf::Text title("PAUSED", game.font, 40);
    title.setFillColor(sf::Color::White);
    title.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 80), 150.0f);
    game.window.draw(title);

    std::vector<std::string> items = { "Continue", "Exit to Menu" };
    for (int i = 0; i < 2; ++i) {
        sf::Text item(items[i], game.font, 30);
        item.setFillColor(i == game.pauseChoice ? sf::Color::Yellow : sf::Color::White);
        item.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 70),
            250.0f + static_cast<float>(i * 60));
        game.window.draw(item);
    }
    game.window.display();
}

void renderGameOver(Game& game) {
    game.window.clear(sf::Color::Black);
    game.window.draw(game.backgroundSprite);
    drawWalls(game);

    sf::Text gameOver("GAME OVER", game.font, 48);
    gameOver.setFillColor(sf::Color::Red);
    gameOver.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 130), 40.0f);
    game.window.draw(gameOver);

    std::stringstream ss;
    ss << "Score: " << game.score;
    sf::Text scoreText(ss.str(), game.font, 30);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 60), 100.0f);
    game.window.draw(scoreText);

    sf::Text recordsTitle("Top 5 Records:", game.font, 24);
    recordsTitle.setFillColor(sf::Color::Cyan);
    recordsTitle.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 100), 140.0f);
    game.window.draw(recordsTitle);

    int yPos = 170;
    int count = 0;
    for (const auto& rec : game.records) {
        if (count >= TOP_RECORDS_POPUP) break;
        std::stringstream line;
        line << count + 1 << ". " << rec.name << "  " << rec.score;
        sf::Text recordLine(line.str(), game.font, 20);
        recordLine.setFillColor(sf::Color::White);
        recordLine.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 120),
            static_cast<float>(yPos));
        game.window.draw(recordLine);
        yPos += 25;
        count++;
    }

    std::vector<std::string> items = { "Play Again", "Main Menu" };
    for (int i = 0; i < 2; ++i) {
        sf::Text item(items[i], game.font, 26);
        item.setFillColor(i == game.gameOverChoice ? sf::Color::Yellow : sf::Color::White);
        item.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 80),
            310.0f + static_cast<float>(i * 40));
        game.window.draw(item);
    }

    game.window.display();
}

void renderEnterName(Game& game) {
    game.window.clear(sf::Color::Black);
    game.window.draw(game.backgroundSprite);
    drawWalls(game);

    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(WINDOW_WIDTH),
        static_cast<float>(WINDOW_HEIGHT)));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    game.window.draw(overlay);

    sf::Text prompt("New Record! Enter your name:", game.font, 30);
    prompt.setFillColor(sf::Color::Yellow);
    prompt.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 180), 180.0f);
    game.window.draw(prompt);

    sf::Text nameText(game.playerName, game.font, 36);
    nameText.setFillColor(sf::Color::White);
    nameText.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 80), 260.0f);
    game.window.draw(nameText);

    sf::Text hint("Press Backspace to delete, Enter to confirm", game.font, 18);
    hint.setFillColor(sf::Color(200, 200, 200));
    hint.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 200), 350.0f);
    game.window.draw(hint);

    game.window.display();
}

void drawMenu(Game& game) {
    game.window.clear(sf::Color(30, 30, 30));

    sf::Text title("SNAKE GAME", game.font, 48);
    title.setFillColor(sf::Color::Green);
    title.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 120), 40.0f);
    game.window.draw(title);

    std::vector<std::string> items = { "Play", "Difficulty", "Records", "Settings", "Exit" };
    for (int i = 0; i < 5; ++i) {
        sf::Text item(items[i], game.font, 30);
        item.setFillColor(i == game.menuSelection ? sf::Color::Yellow : sf::Color::White);
        item.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 70),
            120.0f + static_cast<float>(i * 50));
        game.window.draw(item);
    }

    game.window.display();
}

void drawDifficultySelect(Game& game) {
    game.window.clear(sf::Color(30, 30, 30));

    sf::Text title("Select Difficulty", game.font, 36);
    title.setFillColor(sf::Color::Cyan);
    title.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 130), 40.0f);
    game.window.draw(title);

    std::vector<std::string> levels = { "1 - Easy", "2 - Medium", "3 - Normal", "4 - Hard", "5 - Expert" };
    for (int i = 0; i < 5; ++i) {
        sf::Text item(levels[i], game.font, 28);
        item.setFillColor(i == game.difficultySelection ? sf::Color::Yellow : sf::Color::White);
        item.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 80),
            100.0f + static_cast<float>(i * 45));
        game.window.draw(item);
    }

    sf::Text hint("Press Enter to select, B to return", game.font, 16);
    hint.setFillColor(sf::Color(200, 200, 200));
    hint.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 150),
        static_cast<float>(WINDOW_HEIGHT - 30));
    game.window.draw(hint);

    game.window.display();
}

void drawRecordsTable(Game& game) {
    game.window.clear(sf::Color(30, 30, 30));

    sf::Text title("Records (Top 10)", game.font, 36);
    title.setFillColor(sf::Color::Cyan);
    title.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 130), 40.0f);
    game.window.draw(title);

    int yPos = 90;
    int count = 0;
    for (const auto& rec : game.records) {
        if (count >= TOP_RECORDS_MENU) break;
        std::stringstream line;
        line << count + 1 << ". " << rec.name << "  " << rec.score;
        sf::Text recordLine(line.str(), game.font, 22);
        recordLine.setFillColor(sf::Color::White);
        recordLine.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 120),
            static_cast<float>(yPos));
        game.window.draw(recordLine);
        yPos += 30;
        count++;
    }
    if (count == 0) {
        sf::Text empty("No records yet.", game.font, 22);
        empty.setFillColor(sf::Color(150, 150, 150));
        empty.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 80), 150.0f);
        game.window.draw(empty);
    }

    sf::Text hint("Press B to return", game.font, 16);
    hint.setFillColor(sf::Color(200, 200, 200));
    hint.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 80),
        static_cast<float>(WINDOW_HEIGHT - 30));
    game.window.draw(hint);

    game.window.display();
}

void drawSettings(Game& game) {
    game.window.clear(sf::Color(30, 30, 30));

    sf::Text title("Settings", game.font, 36);
    title.setFillColor(sf::Color::Cyan);
    title.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 70), 40.0f);
    game.window.draw(title);

    std::vector<std::string> items = { "Sound: " + std::string(game.soundEnabled ? "ON" : "OFF"),
                                      "Music: " + std::string(game.musicEnabled ? "ON" : "OFF") };
    for (int i = 0; i < 2; ++i) {
        sf::Text item(items[i], game.font, 28);
        item.setFillColor(i == game.settingsSelection ? sf::Color::Yellow : sf::Color::White);
        item.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 80),
            120.0f + static_cast<float>(i * 60));
        game.window.draw(item);
    }

    sf::Text hint("Use Up/Down to select, Left/Right to toggle, B to return", game.font, 16);
    hint.setFillColor(sf::Color(200, 200, 200));
    hint.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 200),
        static_cast<float>(WINDOW_HEIGHT - 30));
    game.window.draw(hint);

    game.window.display();
}