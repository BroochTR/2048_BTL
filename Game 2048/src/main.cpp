#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <unordered_map>

// Constants
const int GRID_SIZE = 6;
const int TILE_SIZE = 100;
const int WINDOW_WIDTH = GRID_SIZE * TILE_SIZE;
const int WINDOW_HEIGHT = GRID_SIZE * TILE_SIZE;
const int TILE_PADDING = 10;

// Function prototypes
void initializeGrid(std::vector<std::vector<int>>& grid);
void renderGrid(SDL_Renderer* renderer, const std::vector<std::vector<int>>& grid, std::unordered_map<int, SDL_Texture*>& tileTextures);
void spawnTile(std::vector<std::vector<int>>& grid);
bool moveTiles(std::vector<std::vector<int>>& grid, int direction);
bool mergeTiles(std::vector<std::vector<int>>& grid, int direction);
bool isGameOver(const std::vector<std::vector<int>>& grid);
SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer);
std::unordered_map<int, SDL_Texture*> loadTileTextures(SDL_Renderer* renderer);

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("2048 Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    std::unordered_map<int, SDL_Texture*> tileTextures = loadTileTextures(renderer);

    std::vector<std::vector<int>> grid(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));
    initializeGrid(grid);

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN) {
                bool moved = false;
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    moved = moveTiles(grid, 0) || mergeTiles(grid, 0);
                    moveTiles(grid, 0);
                    break;
                case SDLK_DOWN:
                    moved = moveTiles(grid, 1) || mergeTiles(grid, 1);
                    moveTiles(grid, 1);
                    break;
                case SDLK_LEFT:
                    moved = moveTiles(grid, 2) || mergeTiles(grid, 2);
                    moveTiles(grid, 2);
                    break;
                case SDLK_RIGHT:
                    moved = moveTiles(grid, 3) || mergeTiles(grid, 3);
                    moveTiles(grid, 3);
                    break;
                }
                if (moved) {
                    spawnTile(grid);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        renderGrid(renderer, grid, tileTextures);
        SDL_RenderPresent(renderer);

        if (isGameOver(grid)) {
            std::cout << "Game Over!" << std::endl;
            quit = true;
        }
    }

    for (auto& pair : tileTextures) {
        SDL_DestroyTexture(pair.second);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture) {
        std::cerr << "Failed to load texture: " << IMG_GetError() << std::endl;
    }
    return texture;
}

std::unordered_map<int, SDL_Texture*> loadTileTextures(SDL_Renderer* renderer) {
    std::unordered_map<int, SDL_Texture*> textures;
    textures[2] = loadTexture("E:/Test Project/images/2.png", renderer);
    textures[4] = loadTexture("E:/Test Project/images/4.png", renderer);
    textures[8] = loadTexture("E:/Test Project/images/8.png", renderer);
    textures[16] = loadTexture("E:/Test Project/images/16.png", renderer);
    textures[32] = loadTexture("E:/Test Project/images/32.png", renderer);
    textures[64] = loadTexture("E:/Test Project/images/64.png", renderer);
    textures[128] = loadTexture("E:/Test Project/images/128.png", renderer);
    textures[256] = loadTexture("E:/Test Project/images/256.png", renderer);
    textures[512] = loadTexture("E:/Test Project/images/512.png", renderer);
    textures[1024] = loadTexture("E:/Test Project/images/1024.png", renderer);
    textures[2048] = loadTexture("E:/Test Project/images/2048.png", renderer);
    return textures;
}

void initializeGrid(std::vector<std::vector<int>>& grid) {
    std::srand(std::time(nullptr));
    spawnTile(grid);
    spawnTile(grid);
}

void renderGrid(SDL_Renderer* renderer, const std::vector<std::vector<int>>& grid, std::unordered_map<int, SDL_Texture*>& tileTextures) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            int value = grid[i][j];
            SDL_Rect tileRect = { j * TILE_SIZE + TILE_PADDING, i * TILE_SIZE + TILE_PADDING, TILE_SIZE - TILE_PADDING, TILE_SIZE - TILE_PADDING };

            if (value != 0) {
                SDL_Texture* texture = tileTextures[value];
                SDL_RenderCopy(renderer, texture, nullptr, &tileRect);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Background color for empty tiles
                SDL_RenderFillRect(renderer, &tileRect);
            }
        }
    }
}

void spawnTile(std::vector<std::vector<int>>& grid) {
    int x, y;
    do {
        x = std::rand() % GRID_SIZE;
        y = std::rand() % GRID_SIZE;
    } while (grid[x][y] != 0);

    grid[x][y] = (std::rand() % 10 == 0) ? 4 : 2;
}

bool moveTiles(std::vector<std::vector<int>>& grid, int direction) {
    bool moved = false;
    if (direction == 0) { // Up
        for (int j = 0; j < GRID_SIZE; ++j) {
            for (int i = 1; i < GRID_SIZE; ++i) {
                if (grid[i][j] != 0) {
                    int k = i;
                    while (k > 0 && grid[k - 1][j] == 0) {
                        grid[k - 1][j] = grid[k][j];
                        grid[k][j] = 0;
                        k--;
                        moved = true;
                    }
                }
            }
        }
    }
    else if (direction == 1) { // Down
        for (int j = 0; j < GRID_SIZE; ++j) {
            for (int i = GRID_SIZE - 2; i >= 0; --i) {
                if (grid[i][j] != 0) {
                    int k = i;
                    while (k < GRID_SIZE - 1 && grid[k + 1][j] == 0) {
                        grid[k + 1][j] = grid[k][j];
                        grid[k][j] = 0;
                        k++;
                        moved = true;
                    }
                }
            }
        }
    }
    else if (direction == 2) { // Left
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 1; j < GRID_SIZE; ++j) {
                if (grid[i][j] != 0) {
                    int k = j;
                    while (k > 0 && grid[i][k - 1] == 0) {
                        grid[i][k - 1] = grid[i][k];
                        grid[i][k] = 0;
                        k--;
                        moved = true;
                    }
                }
            }
        }
    }
    else if (direction == 3) { // Right
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = GRID_SIZE - 2; j >= 0; --j) {
                if (grid[i][j] != 0) {
                    int k = j;
                    while (k < GRID_SIZE - 1 && grid[i][k + 1] == 0) {
                        grid[i][k + 1] = grid[i][k];
                        grid[i][k] = 0;
                        k++;
                        moved = true;
                    }
                }
            }
        }
    }
    return moved;
}

bool mergeTiles(std::vector<std::vector<int>>& grid, int direction) {
    bool merged = false;
    if (direction == 0) { // Up
        for (int j = 0; j < GRID_SIZE; ++j) {
            for (int i = 1; i < GRID_SIZE; ++i) {
                if (grid[i][j] != 0 && grid[i][j] == grid[i - 1][j]) {
                    grid[i - 1][j] *= 2;
                    grid[i][j] = 0;
                    merged = true;
                }
            }
        }
    }
    else if (direction == 1) { // Down
        for (int j = 0; j < GRID_SIZE; ++j) {
            for (int i = GRID_SIZE - 2; i >= 0; --i) {
                if (grid[i][j] != 0 && grid[i][j] == grid[i + 1][j]) {
                    grid[i + 1][j] *= 2;
                    grid[i][j] = 0;
                    merged = true;
                }
            }
        }
    }
    else if (direction == 2) { // Left
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 1; j < GRID_SIZE; ++j) {
                if (grid[i][j] != 0 && grid[i][j] == grid[i][j - 1]) {
                    grid[i][j - 1] *= 2;
                    grid[i][j] = 0;
                    merged = true;
                }
            }
        }
    }
    else if (direction == 3) { // Right
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = GRID_SIZE - 2; j >= 0; --j) {
                if (grid[i][j] != 0 && grid[i][j] == grid[i][j + 1]) {
                    grid[i][j + 1] *= 2;
                    grid[i][j] = 0;
                    merged = true;
                }
            }
        }
    }
    return merged;
}

bool isGameOver(const std::vector<std::vector<int>>& grid) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (grid[i][j] == 0) return false;
            if (i < GRID_SIZE - 1 && grid[i][j] == grid[i + 1][j]) return false;
            if (j < GRID_SIZE - 1 && grid[i][j] == grid[i][j + 1]) return false;
        }
    }
    return true;
}

