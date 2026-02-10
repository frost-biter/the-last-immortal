#ifndef SPIRIT_ESSENCE_HPP
#define SPIRIT_ESSENCE_HPP

#include <SFML/Graphics.hpp>
#include <Core/AssetManager.hpp>

class SpiritEssence {
public:
    SpiritEssence(sf::Vector2f position);
    bool drain(float amount);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    sf::Vector2f getPosition() const;

private:
    float m_amount;
    sf::Sprite m_sprite;
    
    const Animation* m_anim = nullptr;
    int m_frame;
    float m_timer;
};

#endif