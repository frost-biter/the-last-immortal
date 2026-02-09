#include <Entities/Rival.hpp>
#include <cmath>
#include <cstdlib>
#include <string>

Rival::Rival(sf::Vector2f position) 
    : Entity(15.f, sf::Color::Red, position, 150.f), m_state(RivalState::Wandering), m_stateTimer(0.f) {
    // Randomize starting power slightly
    m_layer = (rand() % 3) + 1; 
    static int id = 1;
    setName("Disciple " + std::to_string(id++));
    
    playAnimation("Rival_Standing");
    m_sprite.setColor(sf::Color::White); // Reset color so it's not tinted red by Entity constructor
}

void Rival::update(float dt, sf::Vector2f mapBounds, sf::Vector2f playerPos, int playerPower) {
    m_stateTimer -= dt;

    // State Machine Logic
    if (m_stateTimer <= 0.f) {
        m_stateTimer = (rand() % 3) + 2.0f; // Change state every 2-5 seconds
        
        // 70% chance to wander, 30% to meditate
        if (rand() % 10 < 7) {
            m_state = RivalState::Wandering;
            // Pick random direction
            float angle = (rand() % 360) * 3.14159f / 180.f;
            m_velocity = {std::cos(angle) * m_speed, std::sin(angle) * m_speed};
            m_sprite.setColor(sf::Color::White);
        } else {
            m_state = RivalState::Meditating;
            m_velocity = {0.f, 0.f};
            m_sprite.setColor(sf::Color(200, 200, 200)); // Slightly darker when meditating
        }
    }

    // AURA MECHANIC: Check for Fleeing
    sf::Vector2f diff = m_shape.getPosition() - playerPos;
    float distSq = diff.x * diff.x + diff.y * diff.y;
    
    // If player is close (< 150px) and stronger
    if (distSq < 22500.f && playerPower > getPowerLevel()) {
        m_state = RivalState::Fleeing;
        // Run away from player
        float len = std::sqrt(distSq);
        if (len > 0) diff /= len;
        m_velocity = diff * (m_speed * 1.5f); // Run fast!
        m_sprite.setColor(sf::Color(255, 150, 150)); // Pale red tint
    }
    if (m_state == RivalState::Wandering || m_state == RivalState::Fleeing) {
        moveAndClamp(dt, mapBounds);
    }

    // Update Animation based on state
    if (m_state == RivalState::Meditating) {
        playAnimation("Rival_Sitting");
    } else if (m_state == RivalState::Wandering || m_state == RivalState::Fleeing) {
        playAnimation("Rival_Walking");
    } else {
        playAnimation("Rival_Standing");
    }
    updateAnimation(dt);
}