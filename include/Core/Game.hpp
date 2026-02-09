#ifndef GAME_HPP
#define GAME_HPP
#include <SFML/Graphics.hpp>
#include <Entities/Player.hpp>
#include <World/WorldMap.hpp>
#include <Entities/SpiritEssence.hpp>
#include <Entities/Rival.hpp>
#include <vector>

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(sf::Time dt);
    void render();

    sf::RenderWindow m_window;
    sf::View m_view;
    float m_shakeTimer;
    Player m_player;
    WorldMap m_map;
    std::vector<SpiritEssence> m_essences;
    float m_essenceSpawnTimer;
    std::vector<Rival> m_rivals;
    std::vector<sf::Vertex> m_drainBeams;
    sf::Text m_rankingText;
};
#endif