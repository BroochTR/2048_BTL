#ifndef TILE_H
#define TILE_H

#include <SDL2/SDL.h>

class Tile {
public:
    int value;
    Tile();
    void draw(SDL_Renderer* renderer, int x, int y, int size);
};

#endif // TILE_H
