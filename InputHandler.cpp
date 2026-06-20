#include "InputHandler.h"
#include "GameLogic.h"
#include "RecordManager.h"
#include "Constants.h"
#include <SFML/Window/Event.hpp>

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
            // Сброс таймера перед игрой
            game.clock.restart();
            game.moveTimer = 0.0f;
            game.started = false;
            game.startTimer = START_DELAY;
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
            // Выход из паузы – сброс таймера и задержка
            game.state = Game::PLAYING;
            game.clock.restart();
            game.moveTimer = 0.0f;
            game.started = false;
            game.startTimer = START_DELAY;
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
            game.clock.restart();
            game.moveTimer = 0.0f;
            game.started = false;
            game.startTimer = START_DELAY;
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
        game.clock.restart();
        game.moveTimer = 0.0f;
        game.started = false;
        game.startTimer = START_DELAY;
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
            game.clock.restart();
            game.moveTimer = 0.0f;
            game.started = false;
            game.startTimer = START_DELAY;
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
    // Обрабатываем только специальные клавиши
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
    // Символы обрабатываются в handleEnterNameTextInput (TextEntered)
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