#ifndef SPIRIT_ESSENCE_HPP
#define SPIRIT_ESSENCE_HPP
#include <SFML/Graphics.hpp>

class SpiritEssence {
public:
    SpiritEssence(sf::Vector2f position);
    void draw(sf::RenderWindow& window);
    bool drain(float amount); // Returns true if empty
    sf::Vector2f getPosition() const;
    float getAmount() const { return m_amount; }

private:
    sf::CircleShape m_shape;
    float m_amount;
};
#endif
