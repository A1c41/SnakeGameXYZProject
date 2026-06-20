#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <deque>
#include <vector>
#include <string>
#include "Constants.h"

struct Record {
    std::string name;
    int score;
};

struct Snake {
    std::deque<sf::Vector2i> body;
    sf::Vector2i direction = sf::Vector2i(1, 0);
    sf::Vector2i nextDirection = sf::Vector2i(1, 0);
    bool alive = true;
};

struct Food {
    sf::Vector2i position;
    sf::Sprite sprite;
};

struct Game {
    sf::RenderWindow window;
    sf::Clock clock;
    float moveTimer = 0.0f;
    float moveInterval = 0.5f;

    Snake snake;
    Food food;
    int score = 0;

    sf::Texture texHead;
    sf::Texture texBody;
    sf::Texture texFood;
    sf::Texture texWall;
    sf::Texture texBackground;
    sf::Sprite backgroundSprite;

    sf::SoundBuffer bufferClick;
    sf::SoundBuffer bufferCrash;
    sf::SoundBuffer bufferStart;
    sf::SoundBuffer bufferEat;
    sf::Sound soundClick;
    sf::Sound soundCrash;
    sf::Sound soundStart;
    sf::Sound soundEat;
    sf::Music music;
    bool soundEnabled = true;
    bool musicEnabled = true;

    enum State {
        MENU,
        DIFFICULTY_SELECT,
        RECORDS_TABLE,
        SETTINGS,
        PLAYING,
        PAUSED,
        GAMEOVER,
        ENTER_NAME
    };
    State state = MENU;

    int difficulty = 1;
    int scorePerApple = 2;

    int menuSelection = 0;
    int settingsSelection = 0;
    int difficultySelection = 0;
    int gameOverChoice = 0;
    int pauseChoice = 0;

    sf::Font font;

    bool paused = false;
    bool started = false;
    float startTimer = START_DELAY;

    std::string playerName = "Player";

    std::vector<Record> records;
    bool newRecord = false;
};