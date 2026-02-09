#include <World/WorldMap.hpp>
#include <Core/ResourceManager.hpp>
#include <Entities/Entity.hpp>
#include <cstdlib>

WorldMap::WorldMap() {
    m_vertices.setPrimitiveType(sf::PrimitiveType::Quads);
    m_vertices.resize(m_width * m_height * 4);

    // Load texture to determine layout (18 patterns implied)
    std::string texPath = "assets/isometric_floor_tiles_128x64/Small 128x64/Exterior/Grass/Floor_Grass_01-128x64.png";
    sf::Texture& texture = ResourceManager::instance().getTexture(texPath);
    unsigned int texW = texture.getSize().x;
    unsigned int texH = texture.getSize().y;
    int cols = (texW / m_tileSize) > 0 ? (texW / m_tileSize) : 1;
    int rows = (texH / (m_tileSize / 2)) > 0 ? (texH / (m_tileSize / 2)) : 1;
    int maxTiles = cols * rows;

    float halfW = m_tileSize / 2.0f;
    float halfH = m_tileSize / 4.0f; // Flattened height for isometric look

    for (int i = 0; i < m_width; ++i) {
        for (int j = 0; j < m_height; ++j) {
            int idx = (i + j * m_width) * 4;

            // Isometric Projection
            // Grid (i, j) -> Screen (x, y)
            sf::Vector2f pos = gridToIso(static_cast<float>(i), static_cast<float>(j));
            float x = pos.x;
            float y = pos.y;

            m_vertices[idx + 0].position = {x, y};                 // Top
            m_vertices[idx + 1].position = {x + halfW, y + halfH}; // Right
            m_vertices[idx + 2].position = {x, y + 2 * halfH};     // Bottom
            m_vertices[idx + 3].position = {x - halfW, y + halfH}; // Left

            // Randomly select one of the patterns from the tileset
            int tileIdx = std::rand() % maxTiles;
            int col = tileIdx % cols;
            int row = tileIdx / cols;
            float u = static_cast<float>(col * m_tileSize);
            float v = static_cast<float>(row * (m_tileSize / 2));

            // Texture Coordinates (Mapping a 128x64 diamond tile)
            m_vertices[idx + 0].texCoords = {u + 64.f, v + 0.f};
            m_vertices[idx + 1].texCoords = {u + 128.f, v + 32.f};
            m_vertices[idx + 2].texCoords = {u + 64.f, v + 64.f};
            m_vertices[idx + 3].texCoords = {u + 0.f, v + 32.f};

            for (int k = 0; k < 4; ++k) m_vertices[idx + k].color = sf::Color::White;
        }
    }
}

void WorldMap::draw(sf::RenderWindow& window) {
    // Using the grass texture as implied by the folder structure
    window.draw(m_vertices, &ResourceManager::instance().getTexture("assets/isometric_floor_tiles_128x64/Small 128x64/Exterior/Grass/Floor_Grass_01-128x64.png"));
}

sf::Vector2f WorldMap::gridToIso(float gridX, float gridY) const {
    float startX = 400.f;
    float startY = 100.f;
    float halfW = m_tileSize / 2.0f;
    float halfH = m_tileSize / 4.0f;
    float x = startX + (gridX - gridY) * halfW;
    float y = startY + (gridX + gridY) * halfH;
    return {x, y};
}

sf::Vector2f WorldMap::isoToGrid(float x, float y) const {
    float startX = 400.f;
    float startY = 100.f;
    float halfW = m_tileSize / 2.0f;
    float halfH = m_tileSize / 4.0f;
    // Inverse logic
    // x - startX = (gx - gy) * hW  => B = gx - gy
    // y - startY = (gx + gy) * hH  => A = gx + gy
    float B = (x - startX) / halfW;
    float A = (y - startY) / halfH;
    float gx = (A + B) / 2.0f;
    float gy = (A - B) / 2.0f;
    return {gx, gy};
}

void WorldMap::clampEntity(Entity& entity) const {
    sf::Vector2f pos = entity.getPosition();
    sf::Vector2f gridPos = isoToGrid(pos.x, pos.y);
    
    // Clamp gridPos
    if (gridPos.x < 0.f) gridPos.x = 0.f;
    if (gridPos.x > m_width - 1) gridPos.x = m_width - 1;
    if (gridPos.y < 0.f) gridPos.y = 0.f;
    if (gridPos.y > m_height - 1) gridPos.y = m_height - 1;
    
    entity.setPosition(gridToIso(gridPos.x, gridPos.y));
}