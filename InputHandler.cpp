#include "InputHandler.h"
#include "GameLogic.h"
#include "RecordManager.h"
#include "GameController.h"
#include "Constants.h"
#include <SFML/Window/Event.hpp>
#include <functional>

static void moveSelection(int& selection, int count, int delta) {
    selection = (selection + delta + count) % count;
}

template<typename OnEnter, typename OnBack>
static void handleMenuNavigation(Game& game, int& selection, int itemCount, const sf::Event& event,
    OnEnter onEnter, OnBack onBack) {
    if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) {
        moveSelection(selection, itemCount, -1);
        if (game.soundEnabled) game.soundClick.play();
    }
    else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) {
        moveSelection(selection, itemCount, 1);
        if (game.soundEnabled) game.soundClick.play();
    }
    else if (event.key.code == sf::Keyboard::Enter) {
        if (game.soundEnabled) game.soundClick.play();
        onEnter();
    }
    else if (event.key.code == sf::Keyboard::B || event.key.code == sf::Keyboard::Escape) {
        if (game.soundEnabled) game.soundClick.play();
        onBack();
    }
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
                handleMenuNavigation(game, game.menuSelection, 5, event,
                    [&]() {
                        switch (game.menuSelection) {
                        case 0: startNewGame(game); break;
                        case 1: game.state = Game::DIFFICULTY_SELECT; game.difficultySelection = game.difficulty - 1; break;
                        case 2: game.state = Game::RECORDS_TABLE; break;
                        case 3: game.state = Game::SETTINGS; game.settingsSelection = 0; break;
                        case 4: game.window.close(); break;
                        }
                    },
                    []() {}
                );
                break;

            case Game::DIFFICULTY_SELECT:
                handleMenuNavigation(game, game.difficultySelection, 5, event,
                    [&]() {
                        game.difficulty = game.difficultySelection + 1;
                        game.state = Game::MENU;
                    },
                    [&]() { game.state = Game::MENU; }
                );
                break;

            case Game::RECORDS_TABLE:
                if (event.key.code == sf::Keyboard::B) {
                    if (game.soundEnabled) game.soundClick.play();
                    game.state = Game::MENU;
                }
                break;

            case Game::SETTINGS:
                handleMenuNavigation(game, game.settingsSelection, 2, event,
                    []() {},
                    [&]() { game.state = Game::MENU; }
                );
                if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right) {
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
                break;

            case Game::PLAYING:
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
                        resumeGame(game);
                    }
                }
                break;

            case Game::PAUSED:
                handleMenuNavigation(game, game.pauseChoice, 2, event,
                    [&]() {
                        if (game.pauseChoice == 0) resumeGame(game);
                        else goToMenu(game);
                    },
                    [&]() { resumeGame(game); }
                );
                break;

            case Game::GAMEOVER:
                handleMenuNavigation(game, game.gameOverChoice, 2, event,
                    [&]() {
                        if (game.gameOverChoice == 0) startNewGame(game);
                        else goToMenu(game);
                    },
                    [&]() { goToMenu(game); }
                );
                break;

            case Game::ENTER_NAME:
                if (event.key.code == sf::Keyboard::BackSpace) {
                    if (!game.playerName.empty()) game.playerName.pop_back();
                }
                else if (event.key.code == sf::Keyboard::Enter) {
                    if (game.playerName.empty()) game.playerName = "Player";
                    updateRecords(game);
                    game.state = Game::GAMEOVER;
                    game.gameOverChoice = 0;
                }
                else if (event.key.code == sf::Keyboard::Escape) {
                    game.playerName = "Player";
                    updateRecords(game);
                    game.state = Game::GAMEOVER;
                    game.gameOverChoice = 0;
                }
                break;
            }
        }

        if (event.type == sf::Event::TextEntered && game.state == Game::ENTER_NAME) {
            if (event.text.unicode < 32 || event.text.unicode >= 128) return;
            char c = static_cast<char>(event.text.unicode);
            if (game.playerName.length() < MAX_NAME_LENGTH)
                game.playerName += c;
        }
    }
}