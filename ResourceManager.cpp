#include "ResourceManager.h"
#include "Constants.h"
#include <cassert>

sf::Texture createFallbackTexture(sf::Color color, int size) {
    sf::Image img;
    img.create(size, size, color);
    sf::Texture tex;
    tex.loadFromImage(img);
    return tex;
}

void loadResources(Game& game) {
    bool fontLoaded = game.font.loadFromFile(RESOURCES + "fonts\\Roboto-Black.ttf");
    if (!fontLoaded) {
        fontLoaded = game.font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    }
    assert(fontLoaded);

    bool headLoaded = game.texHead.loadFromFile(RESOURCES + "head.png");
    assert(headLoaded);
    if (!headLoaded) game.texHead = createFallbackTexture(sf::Color::Green, CELL_SIZE);

    bool bodyLoaded = game.texBody.loadFromFile(RESOURCES + "body.png");
    assert(bodyLoaded);
    if (!bodyLoaded) game.texBody = createFallbackTexture(sf::Color(0, 180, 0), CELL_SIZE);

    bool foodLoaded = game.texFood.loadFromFile(RESOURCES + "Apple.png");
    assert(foodLoaded);
    if (!foodLoaded) game.texFood = createFallbackTexture(sf::Color::Red, CELL_SIZE);

    bool wallLoaded = game.texWall.loadFromFile(RESOURCES + "wall.png");
    assert(wallLoaded);
    if (!wallLoaded) game.texWall = createFallbackTexture(sf::Color(100, 100, 100), CELL_SIZE);

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
    if (clickLoaded) game.soundClick.setBuffer(game.bufferClick);

    bool crashLoaded = game.bufferCrash.loadFromFile(RESOURCES + "Death.wav");
    assert(crashLoaded);
    if (crashLoaded) game.soundCrash.setBuffer(game.bufferCrash);

    bool startLoaded = game.bufferStart.loadFromFile(RESOURCES + "start.wav");
    assert(startLoaded);
    if (startLoaded) game.soundStart.setBuffer(game.bufferStart);

    bool endLoaded = game.bufferEnd.loadFromFile(RESOURCES + "Death.wav");
    assert(endLoaded);
    if (endLoaded) game.soundEnd.setBuffer(game.bufferEnd);

    bool eatLoaded = game.bufferEat.loadFromFile(RESOURCES + "AppleEat.wav");
    assert(eatLoaded);
    if (eatLoaded) game.soundEat.setBuffer(game.bufferEat);

    bool musicLoaded = game.music.openFromFile(RESOURCES + "music.wav");
    assert(musicLoaded);
    if (musicLoaded) {
        game.music.setLoop(true);
        game.music.setVolume(50);
    }
}