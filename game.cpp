#include "game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr) {}

Game::~Game() {}

void Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, 0);
        isRunning = true;

        // Initialize the board
        board.resize(4, std::vector<Tile>(4));
        std::srand(std::time(0));
        addRandomTile();
        addRandomTile();
    } else {
        isRunning = false;
    }
}

void Game::handleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    moveLeft();
                    break;
                case SDLK_RIGHT:
                    moveRight();
                    break;
                case SDLK_UP:
                    moveUp();
                    break;
                case SDLK_DOWN:
                    moveDown();
                    break;
            }
            if (canMove()) {
                addRandomTile();
            }
            break;
    }
}

void Game::update() {
    // Update the game logic if needed
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    int size = 100;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            board[i][j].draw(renderer, j * size, i * size, size);
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

void Game::addRandomTile() {
    std::vector<std::pair<int, int>> emptyTiles;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (board[i][j].value == 0) {
                emptyTiles.emplace_back(i, j);
            }
        }
    }

    if (!emptyTiles.empty()) {
        auto [x, y] = emptyTiles[std::rand() % emptyTiles.size()];
        board[x][y].value = (std::rand() % 2 + 1) * 2;
    }
}

void Game::moveLeft() {
    mergeLeft();
    for (int i = 0; i < 4; ++i) {
        int pos = 0;
        for (int j = 0; j < 4; ++j) {
            if (board[i][j].value != 0) {
                std::swap(board[i][pos++].value, board[i][j].value);
            }
        }
    }
}

void Game::moveRight() {
    mergeRight();
    for (int i = 0; i < 4; ++i) {
        int pos = 3;
        for (int j = 3; j >= 0; --j) {
            if (board[i][j].value != 0) {
                std::swap(board[i][pos--].value, board[i][j].value);
            }
        }
    }
}

void Game::moveUp() {
    mergeUp();
    for (int j = 0; j < 4; ++j) {
        int pos = 0;
        for (int i = 0; i < 4; ++i) {
            if (board[i][j].value != 0) {
                std::swap(board[pos++][j].value, board[i][j].value);
            }
        }
    }
}

void Game::moveDown() {
    mergeDown();
    for (int j = 0; j < 4; ++j) {
        int pos = 3;
        for (int i = 3; i >= 0; --i) {
            if (board[i][j].value != 0) {
                std::swap(board[pos--][j].value, board[i][j].value);
            }
        }
    }
}

void Game::mergeLeft() {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j].value != 0 && board[i][j].value == board[i][j + 1].value) {
                board[i][j].value *= 2;
                board[i][j + 1].value = 0;
            }
        }
    }
}

void Game::mergeRight() {
    for (int i = 0; i < 4; ++i) {
        for (int j = 3; j > 0; --j) {
            if (board[i][j].value != 0 && board[i][j].value == board[i][j - 1].value) {
                board[i][j].value *= 2;
                board[i][j - 1].value = 0;
            }
        }
    }
}

void Game::mergeUp() {
    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 3; ++i) {
            if (board[i][j].value != 0 && board[i][j].value == board[i + 1][j].value) {
                board[i][j].value *= 2;
                board[i + 1][j].value = 0;
            }
        }
    }
}

void Game::mergeDown() {
    for (int j = 0; j < 4; ++j) {
        for (int i = 3; i > 0; --i) {
            if (board[i][j].value != 0 && board[i][j].value == board[i - 1][j].value) {
                board[i][j].value *= 2;
                board[i - 1][j].value = 0;
            }
        }
    }
}

bool Game::canMove() {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (board[i][j].value == 0) return true;
            if (j < 3 && board[i][j].value == board[i][j + 1].value) return true;
            if (i < 3 && board[i][j].value == board[i + 1][j].value) return true;
        }
    }
    return false;
}
