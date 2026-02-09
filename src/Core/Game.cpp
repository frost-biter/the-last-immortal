#include <Core/Game.hpp>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <Core/ResourceManager.hpp>

// 1. Constructor
Game::Game() : m_window(sf::VideoMode({800, 600}), "The Last Immortal"), m_shakeTimer(0.f), m_essenceSpawnTimer(0.f) {
    m_window.setFramerateLimit(60); // Fixes screen glitching/tearing
    m_view = m_window.getDefaultView();
    
    // Spawn some rivals
    m_rivals.emplace_back(sf::Vector2f(200.f, 200.f));
    m_rivals.emplace_back(sf::Vector2f(600.f, 400.f));

    m_rankingText.setFont(ResourceManager::instance().getFont("assets/times.ttf"));
    m_rankingText.setCharacterSize(16);
    m_rankingText.setFillColor(sf::Color::Yellow);
    m_rankingText.setPosition(600.f, 10.f);
}

// 2. Run Loop
void Game::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        sf::Time dt = clock.restart();

        // 1. Check for Window Events (X button, Resize)
        processEvents();

        update(dt);

        // 4. Render
        render();
    }
}

// 3. Process Events - MUST HAVE Game::
void Game::processEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) m_window.close();
    }
}

// 4. Update - MUST HAVE Game::
void Game::update(sf::Time dt) {
    m_player.handleInput();

    // Spawn Spirit Essence
    m_essenceSpawnTimer += dt.asSeconds();
    if (m_essenceSpawnTimer > 3.0f && m_essences.size() < 5) {
        m_essenceSpawnTimer = 0.f;
        sf::Vector2f bounds = m_map.getBounds();
        float x = static_cast<float>(rand() % static_cast<int>(bounds.x));
        float y = static_cast<float>(rand() % static_cast<int>(bounds.y));
        m_essences.emplace_back(sf::Vector2f(x, y));
    }

    // 1. GATHER ENTITIES & RANKING
    std::vector<Entity*> entities;
    entities.push_back(&m_player);
    for (auto& r : m_rivals) entities.push_back(&r);

    // Sort by Power Level (Descending)
    std::sort(entities.begin(), entities.end(), [](Entity* a, Entity* b) {
        return a->getPowerLevel() > b->getPowerLevel();
    });

    // Update Ranking Text
    std::string rankStr = "--- Sect Ranking ---\n";
    for (size_t i = 0; i < entities.size(); ++i) {
        rankStr += std::to_string(i + 1) + ". " + entities[i]->getName() + 
                   " (Lvl " + std::to_string(entities[i]->getPowerLevel()) + ")\n";
    }
    m_rankingText.setString(rankStr);

    // 2. MERGED QI LOGIC (Cave + Essence + Meditation)
    m_drainBeams.clear();
    
    for (auto* ent : entities) {
        // RULE: You must meditate to gain Qi
        if (!ent->isMeditating()) continue;

        float qiGain = 30.f; // Base rate per second

        // CAVE BONUS: Top-Left Zone (0,0 to 200,200)
        sf::Vector2f pos = ent->getPosition();
        if (pos.x < 200.f && pos.y < 200.f) {
            qiGain *= 10.f; // 10x Speed in Cave!
        }

        // ESSENCE BONUS: Siphoning
        auto it = m_essences.begin();
        while (it != m_essences.end()) {
            sf::Vector2f diff = it->getPosition() - ent->getPosition();
            float distSq = diff.x * diff.x + diff.y * diff.y;
            
            if (distSq < 10000.f) { // 100px range
                // Essence Multiplier
                qiGain *= 3.0f;
                
                // Capture position BEFORE erasing
                sf::Vector2f essencePos = it->getPosition();

                // Drain the essence
                float drainAmount = 20.f * dt.asSeconds();
                bool drained = it->drain(drainAmount);

                // Visual Beam
                sf::Color beamColor = (ent == &m_player) ? sf::Color::Cyan : sf::Color::Red;
                m_drainBeams.push_back(sf::Vertex(essencePos, beamColor));
                m_drainBeams.push_back(sf::Vertex(ent->getPosition(), beamColor));

                if (drained) {
                    it = m_essences.erase(it); // Empty
                } else {
                    ++it;
                }
            } else {
                ++it;
            }
        }

        // Apply Final Qi Gain
        ent->modifyQi(qiGain * dt.asSeconds());
    }
    
    // COMBAT: Blast Mechanic
    if (m_player.checkBlast()) {
        sf::Vector2f pPos = m_player.getPosition();
        for (auto& rival : m_rivals) {
            sf::Vector2f diff = rival.getPosition() - pPos;
            float distSq = diff.x * diff.x + diff.y * diff.y;
            if (distSq < 22500.f) { // 150px range
                // Push back
                float len = std::sqrt(distSq);
                if (len > 0) diff /= len;
                rival.applyKnockback(diff * 800.f); // Strong impulse
                
                // Drop Qi
                rival.modifyQi(-20.f);
            }
        }
        m_shakeTimer = 0.2f; // Small shake on blast
    }

    // Update Rivals
    for (auto& rival : m_rivals) {
        rival.update(dt.asSeconds(), m_map.getBounds(), m_player.getPosition(), m_player.getPowerLevel());
        m_map.clampEntity(rival);
    }

    // Player update (multiplier is now 1.0 because siphoning handles the bonus)
    m_player.update(dt.asSeconds(), m_map.getBounds());
    m_map.clampEntity(m_player);

    // Check for breakthrough
    if (m_player.hasBreakthrough()) {
        m_shakeTimer = 0.5f; // Shake for 0.5 seconds
    }

    // Apply screen shake
    if (m_shakeTimer > 0.f) {
        m_shakeTimer -= dt.asSeconds();
        m_view.setCenter(400.f + (rand() % 10 - 5), 300.f + (rand() % 10 - 5));
    } else {
        m_view.setCenter(400.f, 300.f);
    }
}

// 5. Render - MUST HAVE Game::
void Game::render() {
    m_window.clear();
    m_window.setView(m_view);
    m_map.draw(m_window);

    // Draw Cultivation Cave (Safe Zone) - Isometric
    sf::ConvexShape caveZone;
    caveZone.setPointCount(4);
    float halfW = 64.f; // m_tileSize / 2
    float halfH = 32.f; // m_tileSize / 4
    caveZone.setPoint(0, m_map.gridToIso(0, 0)); // Top of 0,0
    caveZone.setPoint(1, m_map.gridToIso(3, 0) + sf::Vector2f(halfW, halfH)); // Right of 3,0
    caveZone.setPoint(2, m_map.gridToIso(3, 3) + sf::Vector2f(0.f, 2*halfH)); // Bottom of 3,3
    caveZone.setPoint(3, m_map.gridToIso(0, 3) + sf::Vector2f(-halfW, halfH)); // Left of 0,3
    caveZone.setFillColor(sf::Color(20, 20, 40, 150)); // Semi-transparent dark blue
    caveZone.setOutlineColor(sf::Color(100, 100, 255));
    caveZone.setOutlineThickness(2.f);
    m_window.draw(caveZone);

    for (auto& essence : m_essences) {
        essence.draw(m_window);
    }
    if (!m_drainBeams.empty()) {
        m_window.draw(&m_drainBeams[0], m_drainBeams.size(), sf::Lines);
    }
    for (auto& rival : m_rivals) {
        rival.draw(m_window);
    }
    m_player.draw(m_window);
    
    // Removed procedural lightning lines to fix "yellow glitch lines"

    // Draw UI (Steady)
    m_window.setView(m_window.getDefaultView());
    m_player.drawUI(m_window);
    m_window.draw(m_rankingText);
    
    m_window.display();
}