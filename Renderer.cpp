#include "Renderer.h"
#include "Constants.h"
#include <sstream>
#include <cmath>

static void drawText(sf::RenderWindow& window, const std::string& str, const sf::Font& font,
    int x, int y, int size, const sf::Color& color = sf::Color::White,
    bool centered = false) {
    sf::Text text(str, font, size);
    text.setFillColor(color);
    if (centered) {
        sf::FloatRect rect = text.getLocalBounds();
        x -= static_cast<int>(rect.width / 2);
        y -= static_cast<int>(rect.height / 2);
    }
    text.setPosition(static_cast<float>(x), static_cast<float>(y));
    window.draw(text);
}

static void drawMenuItem(Game& game, const std::string& label, int y, bool selected, int size = 30) {
    drawText(game.window, label, game.font, WINDOW_WIDTH / 2 - 70, y, size,
        selected ? sf::Color::Yellow : sf::Color::White);
}

void drawBackgroundAndWalls(Game& game) {
    game.window.draw(game.backgroundSprite);
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

void drawRecordsList(Game& game, int maxCount, int startY) {
    int yPos = startY;
    int count = 0;
    for (const auto& rec : game.records) {
        if (count >= maxCount) break;
        std::stringstream line;
        line << count + 1 << ". " << rec.name << "  " << rec.score;
        drawText(game.window, line.str(), game.font, WINDOW_WIDTH / 2 - 120, yPos, 20, sf::Color::White);
        yPos += 25;
        ++count;
    }
}

void renderGame(Game& game) {
    game.window.clear(sf::Color::Black);
    drawBackgroundAndWalls(game);

    game.window.draw(game.food.sprite);

    for (size_t i = 0; i < game.snake.body.size(); ++i) {
        sf::Sprite sprite(i == 0 ? game.texHead : game.texBody);
        sprite.setPosition(static_cast<float>(game.snake.body[i].x * CELL_SIZE),
            static_cast<float>(game.snake.body[i].y * CELL_SIZE));
        game.window.draw(sprite);
    }

    std::stringstream ss;
    ss << "Score: " << game.score;
    drawText(game.window, ss.str(), game.font, 10, 10, 18, sf::Color::Red);

    if (!game.started && game.snake.alive) {
        int remaining = static_cast<int>(std::ceil(game.startTimer));
        drawText(game.window, std::to_string(remaining), game.font,
            WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 30, 48, sf::Color::Yellow, true);
    }

    game.window.display();
}

void renderPause(Game& game) {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(WINDOW_WIDTH),
        static_cast<float>(WINDOW_HEIGHT)));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    game.window.draw(overlay);

    drawText(game.window, "PAUSED", game.font, WINDOW_WIDTH / 2, 150, 40, sf::Color::White, true);

    std::vector<std::string> items = { "Continue", "Exit to Menu" };
    for (int i = 0; i < 2; ++i) {
        drawMenuItem(game, items[i], 250 + i * 60, i == game.pauseChoice, 30);
    }
    game.window.display();
}

void renderGameOver(Game& game) {
    game.window.clear(sf::Color::Black);
    drawBackgroundAndWalls(game);

    drawText(game.window, "GAME OVER", game.font, WINDOW_WIDTH / 2, 40, 48, sf::Color::Red, true);

    std::stringstream ss;
    ss << "Score: " << game.score;
    drawText(game.window, ss.str(), game.font, WINDOW_WIDTH / 2, 100, 30, sf::Color::White, true);

    drawText(game.window, "Top 5 Records:", game.font, WINDOW_WIDTH / 2, 140, 24, sf::Color::Cyan, true);
    drawRecordsList(game, TOP_RECORDS_POPUP, 170);

    std::vector<std::string> items = { "Play Again", "Main Menu" };
    for (int i = 0; i < 2; ++i) {
        drawMenuItem(game, items[i], 310 + i * 40, i == game.gameOverChoice, 26);
    }

    game.window.display();
}

void renderEnterName(Game& game) {
    game.window.clear(sf::Color::Black);
    drawBackgroundAndWalls(game);

    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(WINDOW_WIDTH),
        static_cast<float>(WINDOW_HEIGHT)));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    game.window.draw(overlay);

    drawText(game.window, "New Record! Enter your name:", game.font,
        WINDOW_WIDTH / 2, 180, 30, sf::Color::Yellow, true);
    drawText(game.window, game.playerName, game.font, WINDOW_WIDTH / 2, 260, 36, sf::Color::White, true);
    drawText(game.window, "Press Backspace to delete, Enter to confirm", game.font,
        WINDOW_WIDTH / 2, 350, 18, sf::Color(200, 200, 200), true);

    game.window.display();
}

void drawMenu(Game& game) {
    game.window.clear(sf::Color(30, 30, 30));
    drawText(game.window, "SNAKE GAME", game.font, WINDOW_WIDTH / 2, 40, 48, sf::Color::Green, true);

    std::vector<std::string> items = { "Play", "Difficulty", "Records", "Settings", "Exit" };
    for (int i = 0; i < 5; ++i)
        drawMenuItem(game, items[i], 120 + i * 50, i == game.menuSelection);

    game.window.display();
}

void drawDifficultySelect(Game& game) {
    game.window.clear(sf::Color(30, 30, 30));
    drawText(game.window, "Select Difficulty", game.font, WINDOW_WIDTH / 2, 40, 36, sf::Color::Cyan, true);

    std::vector<std::string> levels = { "1 - Easy", "2 - Medium", "3 - Normal", "4 - Hard", "5 - Expert" };
    for (int i = 0; i < 5; ++i)
        drawMenuItem(game, levels[i], 100 + i * 45, i == game.difficultySelection, 28);

    drawText(game.window, "Press Enter to select, B to return", game.font,
        WINDOW_WIDTH / 2, WINDOW_HEIGHT - 30, 16, sf::Color(200, 200, 200), true);
    game.window.display();
}

void drawRecordsTable(Game& game) {
    game.window.clear(sf::Color(30, 30, 30));
    drawText(game.window, "Records (Top 10)", game.font, WINDOW_WIDTH / 2, 40, 36, sf::Color::Cyan, true);
    drawRecordsList(game, TOP_RECORDS_MENU, 90);
    drawText(game.window, "Press B to return", game.font,
        WINDOW_WIDTH / 2, WINDOW_HEIGHT - 30, 16, sf::Color(200, 200, 200), true);
    game.window.display();
}

void drawSettings(Game& game) {
    game.window.clear(sf::Color(30, 30, 30));
    drawText(game.window, "Settings", game.font, WINDOW_WIDTH / 2, 40, 36, sf::Color::Cyan, true);

    std::vector<std::string> items = {
        "Sound: " + std::string(game.soundEnabled ? "ON" : "OFF"),
        "Music: " + std::string(game.musicEnabled ? "ON" : "OFF")
    };
    for (int i = 0; i < 2; ++i)
        drawMenuItem(game, items[i], 120 + i * 60, i == game.settingsSelection, 28);

    drawText(game.window, "Use Up/Down to select, Left/Right to toggle, B to return", game.font,
        WINDOW_WIDTH / 2, WINDOW_HEIGHT - 30, 16, sf::Color(200, 200, 200), true);
    game.window.display();
}