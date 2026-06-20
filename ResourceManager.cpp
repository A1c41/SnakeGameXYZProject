#include "ResourceManager.h"
#include "Constants.h"
#include <cassert>

static sf::Texture createFallbackTexture(sf::Color color, int size = CELL_SIZE) {
    sf::Image img;
    img.create(size, size, color);
    sf::Texture tex;
    tex.loadFromImage(img);
    return tex;
}

static bool loadTexture(sf::Texture& tex, const std::string& path, sf::Color fallbackColor) {
    if (tex.loadFromFile(path)) return true;
    tex = createFallbackTexture(fallbackColor);
    return false;
}

void loadResources(Game& game) {
    bool fontLoaded = game.font.loadFromFile(RESOURCES + "fonts\\Roboto-Black.ttf");
    if (!fontLoaded)
        fontLoaded = game.font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    assert(fontLoaded);

    loadTexture(game.texHead, RESOURCES + "head.png", sf::Color::Green);
    loadTexture(game.texBody, RESOURCES + "body.png", sf::Color(0, 180, 0));
    loadTexture(game.texFood, RESOURCES + "Apple.png", sf::Color::Red);
    loadTexture(game.texWall, RESOURCES + "wall.png", sf::Color(100, 100, 100));

    if (loadTexture(game.texBackground, RESOURCES + "background.png", sf::Color(20, 20, 20))) {
        game.backgroundSprite.setTexture(game.texBackground);
        game.backgroundSprite.setPosition(0, 0);
        game.backgroundSprite.setScale(
            static_cast<float>(WINDOW_WIDTH) / game.texBackground.getSize().x,
            static_cast<float>(WINDOW_HEIGHT) / game.texBackground.getSize().y
        );
    }
    else {
        game.backgroundSprite.setTexture(game.texBackground);
    }

    game.bufferClick.loadFromFile(RESOURCES + "click.wav") ? game.soundClick.setBuffer(game.bufferClick) : void();
    game.bufferCrash.loadFromFile(RESOURCES + "Death.wav") ? game.soundCrash.setBuffer(game.bufferCrash) : void();
    game.bufferStart.loadFromFile(RESOURCES + "start.wav") ? game.soundStart.setBuffer(game.bufferStart) : void();
    game.bufferEat.loadFromFile(RESOURCES + "AppleEat.wav") ? game.soundEat.setBuffer(game.bufferEat) : void();

    if (game.music.openFromFile(RESOURCES + "music.wav")) {
        game.music.setLoop(true);
        game.music.setVolume(50);
    }
}