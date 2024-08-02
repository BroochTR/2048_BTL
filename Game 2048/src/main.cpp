#include <SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

// Constants
const int GRID_SIZE = 6;
const int TILE_SIZE = 100;
const int WINDOW_WIDTH = GRID_SIZE * TILE_SIZE;
const int WINDOW_HEIGHT = GRID_SIZE * TILE_SIZE;
const int TILE_PADDING = 10;

// Function prototypes
void initializeGrid(std::vector<std::vector<int>>& grid);
void renderGrid(SDL_Renderer* renderer, const std::vector<std::vector<int>>& grid);
void spawnTile(std::vector<std::vector<int>>& grid);
bool moveTiles(std::vector<std::vector<int>>& grid, int direction);
bool mergeTiles(std::vector<std::vector<int>>& grid, int direction);
bool isGameOver(const std::vector<std::vector<int>>& grid);

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
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
                    moved = moveTiles(grid, 0) | mergeTiles(grid, 0);
                    moveTiles(grid, 0);
                    break;
                case SDLK_DOWN:
                    moved = moveTiles(grid, 1) | mergeTiles(grid, 1);
                    moveTiles(grid, 1);
                    break;
                case SDLK_LEFT:
                    moved = moveTiles(grid, 2) | mergeTiles(grid, 2);
                    moveTiles(grid, 2);
                    break;
                case SDLK_RIGHT:
                    moved = moveTiles(grid, 3) | mergeTiles(grid, 3);
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
        renderGrid(renderer, grid);
        SDL_RenderPresent(renderer);

        if (isGameOver(grid)) {
            std::cout << "Game Over!" << std::endl;
            quit = true;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void initializeGrid(std::vector<std::vector<int>>& grid) {
    std::srand(std::time(nullptr));
    spawnTile(grid);
    spawnTile(grid);
}

void renderGrid(SDL_Renderer* renderer, const std::vector<std::vector<int>>& grid) {
    SDL_Color colors[] = {
        {200, 200, 200, 255}, // Empty
        {238, 228, 218, 255}, {237, 224, 200, 255}, {242, 177, 121, 255},
        {245, 149, 99, 255},  {246, 124, 95, 255},  {246, 94, 59, 255},
        {237, 207, 114, 255}, {237, 204, 97, 255},  {237, 200, 80, 255},
        {237, 197, 63, 255},  {237, 194, 46, 255}
    };

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            int value = grid[i][j];
            int colorIndex = value > 0 ? (int)(std::log2(value)) : 0;

            SDL_SetRenderDrawColor(renderer, colors[colorIndex].r, colors[colorIndex].g, colors[colorIndex].b, colors[colorIndex].a);
            SDL_Rect tile = { j * TILE_SIZE + TILE_PADDING, i * TILE_SIZE + TILE_PADDING, TILE_SIZE - TILE_PADDING, TILE_SIZE - TILE_PADDING };
            SDL_RenderFillRect(renderer, &tile);

            if (value != 0) {
                // Render text for the tile (requires SDL_ttf for rendering text)
                // In this example, text rendering is omitted for simplicity.
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

