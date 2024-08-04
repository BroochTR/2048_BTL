#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <unordered_map>

const int GRID_SIZE = 5;
const int TILE_SIZE = 100;
const int TILE_PADDING = 10;
const int BORDER_THICKNESS = 10; // Uniform border thickness
const int EXTRA_WIDTH = 250; // Extra width for additional area
const int WINDOW_WIDTH = GRID_SIZE * TILE_SIZE + EXTRA_WIDTH;
const int WINDOW_HEIGHT = GRID_SIZE * TILE_SIZE;
void initializeGrid(std::vector<std::vector<int>>& grid);
void renderGrid(SDL_Renderer* renderer, const std::vector<std::vector<int>>& grid, std::unordered_map<int, SDL_Texture*>& tileTextures);
void spawnTile(std::vector<std::vector<int>>& grid);
bool moveAndMergeTiles(std::vector<std::vector<int>>& grid, int direction);
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
                bool movedOrMerged = false;
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    movedOrMerged = moveAndMergeTiles(grid, 0);
                    break;
                case SDLK_DOWN:
                    movedOrMerged = moveAndMergeTiles(grid, 1);
                    break;
                case SDLK_LEFT:
                    movedOrMerged = moveAndMergeTiles(grid, 2);
                    break;
                case SDLK_RIGHT:
                    movedOrMerged = moveAndMergeTiles(grid, 3);
                    break;
                }
                if (movedOrMerged) {
                    spawnTile(grid);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        SDL_Rect extraArea = { GRID_SIZE * TILE_SIZE, 0, EXTRA_WIDTH, WINDOW_HEIGHT };
        SDL_SetRenderDrawColor(renderer, 253, 222, 179, 255); // #fddeb3
        SDL_RenderFillRect(renderer, &extraArea);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for the border
        SDL_Rect extraAreaBorder = { GRID_SIZE * TILE_SIZE - BORDER_THICKNESS, -BORDER_THICKNESS, EXTRA_WIDTH + BORDER_THICKNESS, WINDOW_HEIGHT + BORDER_THICKNESS };
        SDL_RenderDrawRect(renderer, &extraAreaBorder);
        renderGrid(renderer, grid, tileTextures);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for the border
        SDL_Rect gridRect = { 0, 0, GRID_SIZE * TILE_SIZE, GRID_SIZE * TILE_SIZE };
        SDL_RenderDrawRect(renderer, &gridRect);
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
            SDL_Rect tileRect = { j * TILE_SIZE + TILE_PADDING, i * TILE_SIZE + TILE_PADDING, TILE_SIZE - TILE_PADDING * 2, TILE_SIZE - TILE_PADDING * 2 };
            if (value != 0) {
                SDL_Texture* texture = tileTextures[value];
                SDL_RenderCopy(renderer, texture, nullptr, &tileRect);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); 
                SDL_RenderFillRect(renderer, &tileRect);
            }
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
            SDL_Rect innerBorderRect = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_RenderDrawRect(renderer, &innerBorderRect);
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
bool moveAndMergeTiles(std::vector<std::vector<int>>& grid, int direction) {
    bool moved = false;
    bool merged = false;

    auto slideAndMerge = [&](std::vector<int>& line) {
        std::vector<int> newLine(GRID_SIZE, 0);
        int lastIndex = -1;

        for (int i = 0; i < GRID_SIZE; ++i) {
            if (line[i] != 0) {
                int value = line[i];
                if (lastIndex != -1 && newLine[lastIndex] == value) {
                    newLine[lastIndex] *= 2;
                    line[i] = 0;
                    merged = true;
                }
                else {
                    lastIndex = (lastIndex + 1);
                    newLine[lastIndex] = value;
                }
            }
        }

        if (newLine != line) {
            moved = true;
              }
        line = newLine;
        };
    auto processGrid = [&](bool reverse) {
        for (int i = 0; i < GRID_SIZE; ++i) {
            std::vector<int> line(GRID_SIZE);
            for (int j = 0; j < GRID_SIZE; ++j) {
                if (direction == 0) line[j] = grid[j][i];       
                if (direction == 1) line[j] = grid[GRID_SIZE - 1 - j][i]; 
                if (direction == 2) line[j] = grid[i][j];        // Left
                if (direction == 3) line[j] = grid[i][GRID_SIZE - 1 - j]; 
            }
            slideAndMerge(line);
            for (int j = 0; j < GRID_SIZE; ++j) {
                if (direction == 0) grid[j][i] = line[j];       
                if (direction == 1) grid[GRID_SIZE - 1 - j][i] = line[j]; 
                if (direction == 2) grid[i][j] = line[j];     
                if (direction == 3) grid[i][GRID_SIZE - 1 - j] = line[j]; 
            }
        }
     };

    processGrid(direction == 1 || direction == 3);
    return moved || merged;
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
