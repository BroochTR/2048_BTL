#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <vector>
#include "tile.h"

class Game {
public:
    Game();
    ~Game();
    void init(const char* title, int width, int height);
    void handleEvents();
    void update();
    void render();
    void clean();
    bool running() { return isRunning; }
private:
    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<std::vector<Tile>> board;

    void addRandomTile();
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void mergeLeft();
    void mergeRight();
    void mergeUp();
    void mergeDown();
    bool canMove();
};

#endif // GAME_H
