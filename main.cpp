#include "E:/VCS Practice File/Temp/game.h"
#include "E:/VCS Practice File/Temp/tile.h"
int main(int argc, char* argv[]) {
    Game* game = new Game();
    game->init("2048 Game", 400, 400);

    while (game->running()) {
        game->handleEvents();
        game->update();
        game->render();
    }

    game->clean();
    delete game;

    return 0;
}