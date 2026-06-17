#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <deque>
#include <vector>
#include <string>

struct Record {
    std::string name;
    int score;
};

struct Snake {
    std::deque<sf::Vector2i> body;
    sf::Vector2i direction;
    sf::Vector2i nextDirection;
    bool alive;
};

struct Food {
    sf::Vector2i position;
    sf::Sprite sprite;
};

struct Game {
    sf::RenderWindow window;
    sf::Clock clock;
    float moveTimer;
    float moveInterval;

    Snake snake;
    Food food;

    int score;

    sf::Texture texHead;
    sf::Texture texBody;
    sf::Texture texFood;
    sf::Texture texWall;
    sf::Texture texBackground;

    sf::Sprite backgroundSprite;

    sf::SoundBuffer bufferClick;
    sf::SoundBuffer bufferCrash;
    sf::SoundBuffer bufferStart;
    sf::SoundBuffer bufferEnd;
    sf::SoundBuffer bufferEat;
    sf::Sound soundClick;
    sf::Sound soundCrash;
    sf::Sound soundStart;
    sf::Sound soundEnd;
    sf::Sound soundEat;
    sf::Music music;
    bool soundEnabled;
    bool musicEnabled;

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
    State state;

    int difficulty;
    int scorePerApple;
    float baseSpeed;

    int menuSelection;
    int settingsSelection;
    int difficultySelection;
    int recordsMenuSelection;

    sf::Font font;

    bool paused;
    bool started;
    float startTimer;
    float pauseTimer;

    bool enteringName;
    std::string playerName;
    int nameCursor;

    std::vector<Record> records;
    bool newRecord;

    int gameOverChoice;
    int pauseChoice;
};