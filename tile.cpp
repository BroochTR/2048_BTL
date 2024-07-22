#include "tile.h"

Tile::Tile() : value(0) {}

void Tile::draw(SDL_Renderer* renderer, int x, int y, int size) {
    SDL_Rect rect = { x, y, size, size };
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, &rect);

    if (value > 0) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        
    }
}
