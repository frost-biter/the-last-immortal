#ifndef WORLDMAP_HPP
#define WORLDMAP_HPP
#include <SFML/Graphics.hpp>
#include <vector>

class Entity; // Forward declaration

class WorldMap {
public:
    WorldMap();
    void draw(sf::RenderWindow& window);
    sf::Vector2f getBounds() const {
        // Approximate bounds for the isometric diamond (width, height)
        // This is a simplification for the existing collision logic
        return sf::Vector2f(static_cast<float>(m_width * m_tileSize), static_cast<float>(m_height * m_tileSize));
    }

    sf::Vector2f gridToIso(float gridX, float gridY) const;
    sf::Vector2f isoToGrid(float x, float y) const;
    void clampEntity(Entity& entity) const;

private:
    sf::VertexArray m_vertices;
    const int m_tileSize = 128; // Updated for 128x64 tiles
    const int m_width = 20;
    const int m_height = 15;
};
#endif