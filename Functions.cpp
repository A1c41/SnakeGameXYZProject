#include "Functions.h"
#include "Constants.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cassert>

sf::Texture createFallbackTexture(sf::Color color, int size) {
    sf::Image img;
    img.create(size, size, color);
    sf::Texture tex;
    tex.loadFromImage(img);
    return tex;
}

bool loadResources(Game& game) {
    bool success = true;

    bool fontLoaded = game.font.loadFromFile(RESOURCES + "fonts\\Roboto-Black.ttf");
    if (!fontLoaded) {
        fontLoaded = game.font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    }
    assert(fontLoaded);
    if (!fontLoaded) success = false;

    bool headLoaded = game.texHead.loadFromFile(RESOURCES + "head.png");
    assert(headLoaded);
    if (!headLoaded)
        game.texHead = createFallbackTexture(sf::Color::Green, CELL_SIZE);

    bool bodyLoaded = game.texBody.loadFromFile(RESOURCES + "body.png");
    assert(bodyLoaded);
    if (!bodyLoaded)
        game.texBody = createFallbackTexture(sf::Color(0, 180, 0), CELL_SIZE);

    bool foodLoaded = game.texFood.loadFromFile(RESOURCES + "Apple.png");
    assert(foodLoaded);
    if (!foodLoaded)
        game.texFood = createFallbackTexture(sf::Color::Red, CELL_SIZE);

    bool wallLoaded = game.texWall.loadFromFile(RESOURCES + "wall.png");
    assert(wallLoaded);
    if (!wallLoaded)
        game.texWall = createFallbackTexture(sf::Color(100, 100, 100), CELL_SIZE);

    bool bgLoaded = game.texBackground.loadFromFile(RESOURCES + "background.png");
    assert(bgLoaded);
    if (!bgLoaded) {
        game.texBackground = createFallbackTexture(sf::Color(20, 20, 20), CELL_SIZE);
    }
    else {
        game.backgroundSprite.setTexture(game.texBackground);
        game.backgroundSprite.setPosition(0, 0);
        game.backgroundSprite.setScale(
            static_cast<float>(WINDOW_WIDTH) / game.texBackground.getSize().x,
            static_cast<float>(WINDOW_HEIGHT) / game.texBackground.getSize().y
        );
    }

    bool clickLoaded = game.bufferClick.loadFromFile(RESOURCES + "click.wav");
    assert(clickLoaded);
    if (!clickLoaded) {}
    else game.soundClick.setBuffer(game.bufferClick);

    bool crashLoaded = game.bufferCrash.loadFromFile(RESOURCES + "Death.wav");
    assert(crashLoaded);
    if (!crashLoaded) {}
    else game.soundCrash.setBuffer(game.bufferCrash);

    bool startLoaded = game.bufferStart.loadFromFile(RESOURCES + "start.wav");
    assert(startLoaded);
    if (!startLoaded) {}
    else game.soundStart.setBuffer(game.bufferStart);

    bool endLoaded = game.bufferEnd.loadFromFile(RESOURCES + "Death.wav");
    assert(endLoaded);
    if (!endLoaded) {}
    else game.soundEnd.setBuffer(game.bufferEnd);

    bool eatLoaded = game.bufferEat.loadFromFile(RESOURCES + "AppleEat.wav");
    assert(eatLoaded);
    if (!eatLoaded) {}
    else game.soundEat.setBuffer(game.bufferEat);

    bool musicLoaded = game.music.openFromFile(RESOURCES + "music.wav");
    assert(musicLoaded);
    if (!musicLoaded) {
        success = false;
    }
    else {
        game.music.setLoop(true);
        game.music.setVolume(50);
    }

    game.soundEnabled = true;
    game.musicEnabled = true;

    return success;
}

void loadRecords(Game& game) {
    game.records.clear();
    std::ifstream file(RECORDS_FILE);
    if (!file.is_open()) return;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string name;
        int score;
        if (std::getline(ss, name, ';') && (ss >> score)) {
            game.records.push_back({ name, score });
        }
    }
    file.close();
    std::sort(game.records.begin(), game.records.end(),
        [](const Record& a, const Record& b) { return a.score > b.score; });
}

void saveRecords(const Game& game) {
    std::ofstream file(RECORDS_FILE);
    if (!file.is_open()) return;
    for (const auto& rec : game.records) {
        file << rec.name << ";" << rec.score << "\n";
    }
    file.close();
}

bool isHighScore(const Game& game, int score) {
    if (game.records.size() < TOP_RECORDS_POPUP) return true;
    return score > game.records.back().score;
}

void updateRecords(Game& game) {
    if (!game.newRecord) return;
    game.records.push_back({ game.playerName, game.score });
    std::sort(game.records.begin(), game.records.end(),
        [](const Record& a, const Record& b) { return a.score > b.score; });
    saveRecords(game);
    game.newRecord = false;
}

sf::Vector2i getRandomFreePosition(const Game& game) {
    int attempts = 0;
    while (attempts < 1000) {
        int x = 1 + rand() % (GRID_WIDTH - 2);
        int y = 1 + rand() % (GRID_HEIGHT - 2);
        sf::Vector2i pos(x, y);

        bool free = true;
        for (auto& seg : game.snake.body) {
            if (seg == pos) { free = false; break; }
        }
        if (free && pos != game.food.position) {
            return pos;
        }
        attempts++;
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

    game.score = 0;

    float baseInterval = 0.5f;
    switch (game.difficulty) {
    case 1: baseInterval = 0.5f; game.scorePerApple = 2; break;
    case 2: baseInterval = 0.4f; game.scorePerApple = 4; break;
    case 3: baseInterval = 0.3f; game.scorePerApple = 6; break;
    case 4: baseInterval = 0.22f; game.scorePerApple = 8; break;
    case 5: baseInterval = 0.15f; game.scorePerApple = 10; break;
    default: baseInterval = 0.5f; game.scorePerApple = 2;
    }
    game.moveInterval = baseInterval;
    game.baseSpeed = baseInterval;

    game.food.position = getRandomFreePosition(game);
    game.food.sprite = sf::Sprite(game.texFood);
    game.food.sprite.setPosition(static_cast<float>(game.food.position.x * CELL_SIZE),
        static_cast<float>(game.food.position.y * CELL_SIZE));

    game.moveTimer = 0.0f;
    game.started = false;
    game.startTimer = START_DELAY;
    game.paused = false;
    game.pauseTimer = 0.0f;

    game.newRecord = false;
    game.enteringName = false;
    game.playerName = "Player";
}

void spawnFood(Game& game) {
    game.food.position = getRandomFreePosition(game);
    game.food.sprite.setPosition(static_cast<float>(game.food.position.x * CELL_SIZE),
        static_cast<float>(game.food.position.y * CELL_SIZE));
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
            game.enteringName = true;
            game.state = Game::ENTER_NAME;
            game.playerName = "Player";
            game.nameCursor = 0;
        }
        else {
            game.state = Game::GAMEOVER;
        }
    }
}

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
    //drawGrid(game.window);
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
    //renderGame(game);
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
    //drawGrid(game.window);
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
    //drawGrid(game.window);
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
            120.0f + static_cast<float>(i * 50));  // шаг 50
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
            100.0f + static_cast<float>(i * 45));  // шаг 45
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
        yPos += 30;  // шаг 30
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
            120.0f + static_cast<float>(i * 60));  // шаг 60
        game.window.draw(item);
    }

    sf::Text hint("Use Up/Down to select, Left/Right to toggle, B to return", game.font, 16);
    hint.setFillColor(sf::Color(200, 200, 200));
    hint.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 200),
        static_cast<float>(WINDOW_HEIGHT - 30));
    game.window.draw(hint);

    game.window.display();
}

void handleInput(Game& game) {
    sf::Event event;
    while (game.window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            game.window.close();
            return;
        }
        if (event.type == sf::Event::KeyPressed) {
            switch (game.state) {
            case Game::MENU:
                handleMenuInput(game, event);
                break;
            case Game::DIFFICULTY_SELECT:
                handleDifficultyInput(game, event);
                break;
            case Game::RECORDS_TABLE:
                handleRecordsInput(game, event);
                break;
            case Game::SETTINGS:
                handleSettingsInput(game, event);
                break;
            case Game::PLAYING:
                handlePlayingInput(game, event);
                break;
            case Game::PAUSED:
                handlePauseInput(game, event);
                break;
            case Game::GAMEOVER:
                handleGameOverInput(game, event);
                break;
            case Game::ENTER_NAME:
                handleEnterNameInput(game, event);
                break;
            }
        }
        if (event.type == sf::Event::TextEntered) {
            if (game.state == Game::ENTER_NAME) {
                handleEnterNameTextInput(game, event);
            }
        }
    }
}

void handleMenuInput(Game& game, const sf::Event& event) {
    if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) {
        game.menuSelection = (game.menuSelection - 1 + 5) % 5;
        if (game.soundEnabled) game.soundClick.play();
    }
    else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) {
        game.menuSelection = (game.menuSelection + 1) % 5;
        if (game.soundEnabled) game.soundClick.play();
    }
    else if (event.key.code == sf::Keyboard::Enter) {
        if (game.soundEnabled) game.soundClick.play();
        switch (game.menuSelection) {
        case 0:
            initGame(game);
            game.state = Game::PLAYING;
            if (game.musicEnabled && game.music.getStatus() != sf::Music::Playing)
                game.music.play();
            break;
        case 1:
            game.state = Game::DIFFICULTY_SELECT;
            game.difficultySelection = game.difficulty - 1;
            break;
        case 2:
            game.state = Game::RECORDS_TABLE;
            break;
        case 3:
            game.state = Game::SETTINGS;
            game.settingsSelection = 0;
            break;
        case 4:
            game.window.close();
            break;
        }
    }
}

void handleDifficultyInput(Game& game, const sf::Event& event) {
    if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) {
        game.difficultySelection = (game.difficultySelection - 1 + 5) % 5;
        if (game.soundEnabled) game.soundClick.play();
    }
    else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) {
        game.difficultySelection = (game.difficultySelection + 1) % 5;
        if (game.soundEnabled) game.soundClick.play();
    }
    else if (event.key.code == sf::Keyboard::Enter) {
        game.difficulty = game.difficultySelection + 1;
        if (game.soundEnabled) game.soundClick.play();
        game.state = Game::MENU;
    }
    else if (event.key.code == sf::Keyboard::B) {
        game.state = Game::MENU;
        if (game.soundEnabled) game.soundClick.play();
    }
}

void handleRecordsInput(Game& game, const sf::Event& event) {
    if (event.key.code == sf::Keyboard::B) {
        game.state = Game::MENU;
        if (game.soundEnabled) game.soundClick.play();
    }
}

void handleSettingsInput(Game& game, const sf::Event& event) {
    if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) {
        game.settingsSelection = (game.settingsSelection - 1 + 2) % 2;
        if (game.soundEnabled) game.soundClick.play();
    }
    else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) {
        game.settingsSelection = (game.settingsSelection + 1) % 2;
        if (game.soundEnabled) game.soundClick.play();
    }
    else if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right) {
        if (game.settingsSelection == 0) {
            game.soundEnabled = !game.soundEnabled;
            if (game.soundEnabled) game.soundClick.play();
        }
        else {
            game.musicEnabled = !game.musicEnabled;
            if (game.musicEnabled) {
                if (game.music.getStatus() != sf::Music::Playing && game.state == Game::PLAYING)
                    game.music.play();
            }
            else {
                game.music.pause();
            }
            if (game.soundEnabled) game.soundClick.play();
        }
    }
    else if (event.key.code == sf::Keyboard::B) {
        game.state = Game::MENU;
        if (game.soundEnabled) game.soundClick.play();
    }
}

void handlePlayingInput(Game& game, const sf::Event& event) {
    if (event.key.code == sf::Keyboard::W) {
        if (game.snake.direction.y != 1)
            game.snake.nextDirection = sf::Vector2i(0, -1);
    }
    else if (event.key.code == sf::Keyboard::S) {
        if (game.snake.direction.y != -1)
            game.snake.nextDirection = sf::Vector2i(0, 1);
    }
    else if (event.key.code == sf::Keyboard::A) {
        if (game.snake.direction.x != 1)
            game.snake.nextDirection = sf::Vector2i(-1, 0);
    }
    else if (event.key.code == sf::Keyboard::D) {
        if (game.snake.direction.x != -1)
            game.snake.nextDirection = sf::Vector2i(1, 0);
    }
    else if (event.key.code == sf::Keyboard::P) {
        game.paused = !game.paused;
        if (game.paused) {
            game.pauseChoice = 0;
            game.state = Game::PAUSED;
            if (game.musicEnabled) game.music.pause();
        }
        else {
            game.state = Game::PLAYING;
            game.pauseTimer = START_DELAY;
            if (game.musicEnabled) game.music.play();
        }
    }
}

void handlePauseInput(Game& game, const sf::Event& event) {
    if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) {
        game.pauseChoice = (game.pauseChoice - 1 + 2) % 2;
        if (game.soundEnabled) game.soundClick.play();
    }
    else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) {
        game.pauseChoice = (game.pauseChoice + 1) % 2;
        if (game.soundEnabled) game.soundClick.play();
    }
    else if (event.key.code == sf::Keyboard::Enter) {
        if (game.pauseChoice == 0) {
            game.paused = false;
            game.state = Game::PLAYING;
            game.pauseTimer = START_DELAY;
            if (game.musicEnabled) game.music.play();
            if (game.soundEnabled) game.soundClick.play();
        }
        else {
            game.state = Game::MENU;
            game.paused = false;
            if (game.musicEnabled) game.music.stop();
            if (game.soundEnabled) game.soundClick.play();
        }
    }
    else if (event.key.code == sf::Keyboard::B || event.key.code == sf::Keyboard::Escape) {
        game.paused = false;
        game.state = Game::PLAYING;
        game.pauseTimer = START_DELAY;
        if (game.musicEnabled) game.music.play();
    }
}

void handleGameOverInput(Game& game, const sf::Event& event) {
    if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) {
        game.gameOverChoice = (game.gameOverChoice - 1 + 2) % 2;
        if (game.soundEnabled) game.soundClick.play();
    }
    else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) {
        game.gameOverChoice = (game.gameOverChoice + 1) % 2;
        if (game.soundEnabled) game.soundClick.play();
    }
    else if (event.key.code == sf::Keyboard::Enter) {
        if (game.gameOverChoice == 0) {
            initGame(game);
            game.state = Game::PLAYING;
            if (game.musicEnabled) game.music.play();
            if (game.soundEnabled) game.soundClick.play();
        }
        else {
            game.state = Game::MENU;
            if (game.musicEnabled) game.music.stop();
            if (game.soundEnabled) game.soundClick.play();
        }
    }
    else if (event.key.code == sf::Keyboard::B || event.key.code == sf::Keyboard::Escape) {
        game.state = Game::MENU;
        if (game.musicEnabled) game.music.stop();
    }
}

void handleEnterNameInput(Game& game, const sf::Event& event) {
    if (event.key.code == sf::Keyboard::BackSpace) {
        if (!game.playerName.empty())
            game.playerName.pop_back();
    }
    else if (event.key.code == sf::Keyboard::Enter) {
        if (game.playerName.empty()) game.playerName = "Player";
        updateRecords(game);
        game.enteringName = false;
        game.state = Game::GAMEOVER;
        game.gameOverChoice = 0;
    }
    else if (event.key.code == sf::Keyboard::Escape) {
        game.playerName = "Player";
        updateRecords(game);
        game.enteringName = false;
        game.state = Game::GAMEOVER;
        game.gameOverChoice = 0;
    }
    else {
        if (event.text.unicode < 128 && event.text.unicode >= 32) {
            char c = static_cast<char>(event.text.unicode);
            if (game.playerName.length() < 15)
                game.playerName += c;
        }
    }
}

void handleEnterNameTextInput(Game& game, const sf::Event& event) {
    if (event.text.unicode < 32) return;
    if (event.text.unicode < 128) {
        char c = static_cast<char>(event.text.unicode);
        if (game.playerName.length() < 15) {
            game.playerName += c;
        }
    }
}