#include <Core/Game.hpp>
#include <cstdlib>
#include <ctime>

int main() {
    // Seed random number generator here so WorldMap (initialized in Game constructor) uses it
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    Game game;
    game.run();
    return 0;
}