#pragma once
#include "Structures.h"

bool loadResources(Game& game);
sf::Texture createFallbackTexture(sf::Color color, int size = CELL_SIZE);