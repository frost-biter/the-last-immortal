#ifndef ENTITY_HPP
#define ENTITY_HPP
#include <SFML/Graphics.hpp>
#include <string>
#include <Core/AssetManager.hpp>

enum class Realm {
    QiCondensation,
    FoundationEstablishment
};

class Entity {
public:
    Entity(float radius, sf::Color color, sf::Vector2f position, float speed);
    virtual ~Entity() = default;

    virtual void draw(sf::RenderWindow& window);
    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f pos);
    
    void setTexture(const std::string& path);
    void playAnimation(const std::string& name);
    void updateAnimation(float dt);
    void setName(const std::string& name) { m_name = name; }
    std::string getName() const { return m_name; }
    virtual bool isMeditating() const { return false; }

    // Shared Stats
    int getPowerLevel() const { return static_cast<int>(m_realm) * 10 + m_layer; }
    void modifyQi(float amount);
    void applyKnockback(sf::Vector2f force);

protected:
    // Helper to move (clamping handled by WorldMap now)
    void moveAndClamp(float dt, sf::Vector2f mapBounds);

    sf::Sprite m_sprite;
    sf::CircleShape m_shape;
    sf::Vector2f m_velocity;
    sf::Vector2f m_knockback;
    float m_speed;
    
    Realm m_realm;
    int m_layer;
    float m_currentQi;
    float m_maxQi;
    std::string m_name;

    // Animation State
    const Animation* m_currentAnim = nullptr;
    int m_animFrame = 0;
    float m_animTimer = 0.f;
};
#endif