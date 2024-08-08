#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <unordered_map>

// Constants
const int GRID_SIZE = 5;
const int TILE_SIZE = 100;
const int TILE_PADDING = 10;
const int BORDER_THICKNESS = 10; // Uniform border thickness
const int EXTRA_WIDTH = 250; // Extra width for additional area
const int WINDOW_WIDTH = GRID_SIZE * TILE_SIZE + EXTRA_WIDTH;
const int WINDOW_HEIGHT = GRID_SIZE * TILE_SIZE;

// Function prototypes
void initializeGrid(std::vector<std::vector<int>>& grid);
void renderGrid(SDL_Renderer* renderer, const std::vector<std::vector<int>>& grid, std::unordered_map<int, SDL_Texture*>& tileTextures);
void spawnTile(std::vector<std::vector<int>>& grid);
bool moveAndMergeTiles(std::vector<std::vector<int>>& grid, int direction, int& score);
bool isGameOver(const std::vector<std::vector<int>>& grid);
SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer);
std::unordered_map<int, SDL_Texture*> loadTileTextures(SDL_Renderer* renderer);
SDL_Texture* renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color);

int main(int argc, char* argv[]) {
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    // Load the font
    TTF_Font* font = TTF_OpenFont("E:/2048-font/2048-font.ttf", 24); // Replace with your actual font path
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

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

    // Create the "Score:" text texture
    SDL_Color textColor = { 0, 0, 0, 255 }; // Black color for text
    int score = 0;
    SDL_Texture* scoreLabelTexture = renderText(renderer, font, "Score", textColor);
    SDL_Texture* scoreValueTexture = renderText(renderer, font, "0", textColor);

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
                    movedOrMerged = moveAndMergeTiles(grid, 0, score);
                    break;
                case SDLK_DOWN:
                    movedOrMerged = moveAndMergeTiles(grid, 1, score);
                    break;
                case SDLK_LEFT:
                    movedOrMerged = moveAndMergeTiles(grid, 2, score);
                    break;
                case SDLK_RIGHT:
                    movedOrMerged = moveAndMergeTiles(grid, 3, score);
                    break;
                }
                if (movedOrMerged) {
                    spawnTile(grid);
                    // Update score text texture
                    SDL_DestroyTexture(scoreValueTexture);
                    scoreValueTexture = renderText(renderer, font, std::to_string(score), textColor);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Render the extra area on the right side
        SDL_Rect extraArea = { GRID_SIZE * TILE_SIZE, 0, EXTRA_WIDTH, WINDOW_HEIGHT };
        SDL_SetRenderDrawColor(renderer, 253, 222, 179, 255); // #fddeb3
        SDL_RenderFillRect(renderer, &extraArea);

        // Render the border around the extra area
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for the border
        SDL_Rect extraAreaBorder = { GRID_SIZE * TILE_SIZE - BORDER_THICKNESS, -BORDER_THICKNESS, EXTRA_WIDTH + BORDER_THICKNESS, WINDOW_HEIGHT + BORDER_THICKNESS };
        SDL_RenderDrawRect(renderer, &extraAreaBorder);

        // Render the "Score:" label
        int labelWidth, labelHeight;
        SDL_QueryTexture(scoreLabelTexture, nullptr, nullptr, &labelWidth, &labelHeight);
        SDL_Rect scoreLabelRect = { GRID_SIZE * TILE_SIZE + (EXTRA_WIDTH - labelWidth) / 2, 100, labelWidth, labelHeight };
        SDL_RenderCopy(renderer, scoreLabelTexture, nullptr, &scoreLabelRect);

        // Render the score value
        int valueWidth, valueHeight;
        SDL_QueryTexture(scoreValueTexture, nullptr, nullptr, &valueWidth, &valueHeight);
        SDL_Rect scoreValueRect = { GRID_SIZE * TILE_SIZE + (EXTRA_WIDTH - valueWidth) / 2, 100 + labelHeight + 10, valueWidth, valueHeight }; // Positioned below the label
        SDL_RenderCopy(renderer, scoreValueTexture, nullptr, &scoreValueRect);

        // Render the grid
        renderGrid(renderer, grid, tileTextures);

        // Render the border around the grid
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

    SDL_DestroyTexture(scoreLabelTexture);
    SDL_DestroyTexture(scoreValueTexture);
    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
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

SDL_Texture* renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) {
        std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cerr << "Failed to create text texture: " << SDL_GetError() << std::endl;
    }
    return texture;
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

            // Draw tile background
            if (value != 0) {
                SDL_Texture* texture = tileTextures[value];
                SDL_RenderCopy(renderer, texture, nullptr, &tileRect);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Background color for empty tiles
                SDL_RenderFillRect(renderer, &tileRect);
            }

            // Draw inner border around each tile
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for the border
            SDL_Rect innerBorderRect = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_RenderDrawRect(renderer, &innerBorderRect);
        }
    }
}

void spawnTile(std::vector<std::vector<int>>& grid) {
    // Function to get a random tile value from 2 to 512 (powers of 2)
    auto getRandomTileValue = []() -> int {
        int values[] = { 2, 4, 8, 16, 32 };
        return values[std::rand() % (sizeof(values) / sizeof(values[0]))];
    };

    // Place the tile with the number 2
    int x, y;
    do {
        x = std::rand() % GRID_SIZE;
        y = std::rand() % GRID_SIZE;
    } while (grid[x][y] != 0);

    grid[x][y] = 2;

    // Place the extra random tile
    int extraX, extraY;
    do {
        extraX = std::rand() % GRID_SIZE;
        extraY = std::rand() % GRID_SIZE;
    } while (grid[extraX][extraY] != 0);

    grid[extraX][extraY] = getRandomTileValue();
}

bool moveAndMergeTiles(std::vector<std::vector<int>>& grid, int direction, int& score) {
    bool moved = false;
    bool merged = false;

    auto slideAndMerge = [&](std::vector<int>& line) -> int {
        std::vector<int> newLine(GRID_SIZE, 0);
        int lastIndex = -1;
        int lineScore = 0;

        for (int i = 0; i < GRID_SIZE; ++i) {
            if (line[i] != 0) {
                int value = line[i];
                if (lastIndex != -1 && newLine[lastIndex] == value) {
                    newLine[lastIndex] *= 2;
                    line[i] = 0;
                    lineScore += newLine[lastIndex]; // Update score with the merged value
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
        return lineScore;
    };

    auto processGrid = [&](bool reverse) {
        for (int i = 0; i < GRID_SIZE; ++i) {
            std::vector<int> line(GRID_SIZE);
            for (int j = 0; j < GRID_SIZE; ++j) {
                if (direction == 0) line[j] = grid[j][i];        // Up
                if (direction == 1) line[j] = grid[GRID_SIZE - 1 - j][i]; // Down
                if (direction == 2) line[j] = grid[i][j];        // Left
                if (direction == 3) line[j] = grid[i][GRID_SIZE - 1 - j]; // Right
            }

            int lineScore = slideAndMerge(line);

            for (int j = 0; j < GRID_SIZE; ++j) {
                if (direction == 0) grid[j][i] = line[j];        // Up
                if (direction == 1) grid[GRID_SIZE - 1 - j][i] = line[j]; // Down
                if (direction == 2) grid[i][j] = line[j];        // Left
                if (direction == 3) grid[i][GRID_SIZE - 1 - j] = line[j]; // Right
            }

            score += lineScore;
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

