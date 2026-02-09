#include <Entities/Player.hpp>
#include <Core/ResourceManager.hpp>
#include <cmath>
#include <string>
#include <cstdio>

Player::Player() 
    : Entity(15.f, sf::Color::Green, {400.f, 300.f}, 200.f), 
      m_isMeditating(false), m_wantsToBlast(false), m_blastCooldown(0.f), m_karma(0),
      m_animTimer(0.f), m_currentFrame(0), m_lightningAnimTimer(0.f), m_lightningFrame(1) {
    setName("You");

    // Load Assets
    playAnimation("Cultivator_Standing");

    m_lightningSprite.setTexture(ResourceManager::instance().getTexture("assets/lightning_line_effect/lightning_line_effect/lightning_line_medium/lightning_line5a1.png"));
    m_lightningSprite.setOrigin(m_lightningSprite.getLocalBounds().width / 2, m_lightningSprite.getLocalBounds().height);

    m_qiBarBack.setSize({40.f, 6.f});
    m_qiBarBack.setFillColor(sf::Color(50, 50, 50));

    m_qiBarFill.setSize({0.f, 6.f});
    m_qiBarFill.setFillColor(sf::Color::Cyan);

    // Setup Realm Text
    // Ensure you have arial.ttf or change this to a valid font path
    m_realmText.setFont(ResourceManager::instance().getFont("assets/times.ttf"));
    m_realmText.setCharacterSize(18);
    m_realmText.setFillColor(sf::Color::White);
    m_realmText.setPosition(10.f, 10.f);
    updateRealmText();
}

void Player::handleInput() {
    m_velocity = {0.f, 0.f};
    m_isMeditating = sf::Keyboard::isKeyPressed(sf::Keyboard::C);
    
    // Blast Input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && m_blastCooldown <= 0.f) {
        m_wantsToBlast = true;
        m_blastCooldown = 1.0f; // 1 second cooldown
    }

    if (m_isMeditating) return;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  m_velocity.x -= m_speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) m_velocity.x += m_speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    m_velocity.y -= m_speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  m_velocity.y += m_speed;
}

void Player::update(float dt, sf::Vector2f mapBounds) {
    if (m_blastCooldown > 0.f) m_blastCooldown -= dt;

    // Update Animation State
    if (m_isMeditating) {
        playAnimation("Cultivator_Sitting");
    } else if (m_wantsToBlast) {
        playAnimation("Cultivator_Attacking");
    } else if (std::abs(m_velocity.x) > 0.1f || std::abs(m_velocity.y) > 0.1f) {
        playAnimation("Cultivator_Walking");
    } else {
        playAnimation("Cultivator_Standing");
    }
    updateAnimation(dt);

    // Lightning Animation Logic
    if (m_breakthroughTriggered || (m_isMeditating && m_currentQi > m_maxQi * 0.9f)) {
        m_lightningAnimTimer += dt;
        if (m_lightningAnimTimer > 0.06f) { // Slower animation
            m_lightningAnimTimer = 0.f;
            m_lightningFrame = (m_lightningFrame % 12) + 1;
            std::string path = "assets/lightning_line_effect/lightning_line_effect/lightning_line_medium/lightning_line5a" + std::to_string(m_lightningFrame) + ".png";
            m_lightningSprite.setTexture(ResourceManager::instance().getTexture(path));
        }
    }

    if (m_isMeditating) {
        // CULTIVATION MODE: Change color to show Qi absorption (Time-based now)
        qiProgress += dt; // Accumulate time in seconds
        
        if (m_currentQi >= m_maxQi) {
            m_currentQi = 0.f;
            m_breakthroughTriggered = true;
            
            // Realm Progression
            m_layer++;
            if (m_layer > 9) {
                m_layer = 1;
                if (m_realm == Realm::QiCondensation) m_realm = Realm::FoundationEstablishment;
            }
            
            // Stat Increases
            m_speed += 10.f;
            m_maxQi += 20.f;
            updateRealmText();
        }

        // Speed up the pulse by multiplying qiProgress (e.g., 3.0f for a 2-second cycle)
        float pulse = (std::sin(qiProgress * 5.0f) + 1.0f) * 0.5f; 
        
        // Ensure the Blue channel is always high, and only Green pulses
        m_sprite.setColor(sf::Color(255, 255, 255, 255)); // Reset
        // We could tint the sprite here if we wanted
        
    } else {
        m_sprite.setColor(sf::Color::White);
        moveAndClamp(dt, mapBounds); // Note: mapBounds is ignored by Game::update clamping, but we keep the call for movement
    }

    // Update Qi Bar UI
    sf::Vector2f pos = m_shape.getPosition();
    // We map world position to screen position in drawUI if needed, 
    // but for overhead bars, we usually want them to follow the player in world space.
    // However, the user asked for "Steady" text. The Bar is attached to the player, so it SHOULD shake with the player.
    // The Realm Text is UI, so it should be steady.
    
    // Sync Sprite Position
    // m_sprite.setPosition(m_shape.getPosition()); // Handled by Entity::moveAndClamp / setPosition now
    m_lightningSprite.setPosition(m_shape.getPosition() + sf::Vector2f(0.f, -150.f)); // Appear above character

    // Let's keep the bar attached to the player (World Space)
    m_qiBarBack.setPosition(pos.x - 20.f, pos.y - 30.f);
    m_qiBarFill.setPosition(pos.x - 20.f, pos.y - 30.f);
    float pct = m_currentQi / m_maxQi;
    m_qiBarFill.setSize({40.f * pct, 6.f});
}

bool Player::checkBlast() {
    if (m_wantsToBlast) {
        m_wantsToBlast = false;
        return true;
    }
    return false;
}

bool Player::hasBreakthrough() {
    if (m_breakthroughTriggered) {
        m_breakthroughTriggered = false;
        return true;
    }
    return false;
}

void Player::updateRealmText() {
    std::string r = (m_realm == Realm::QiCondensation) ? "Qi Condensation" : "Foundation Est.";
    m_realmText.setString(r + " - Layer " + std::to_string(m_layer));
}

void Player::draw(sf::RenderWindow& window) {
    Entity::draw(window); // Draws m_sprite from base class
    if (m_breakthroughTriggered || (m_isMeditating && m_currentQi > m_maxQi * 0.9f)) {
        window.draw(m_lightningSprite);
    }
    window.draw(m_qiBarBack);
    window.draw(m_qiBarFill);
}

void Player::drawUI(sf::RenderWindow& window) {
    window.draw(m_realmText);
}