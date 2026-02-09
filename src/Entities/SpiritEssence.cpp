#include <Entities/SpiritEssence.hpp>

SpiritEssence::SpiritEssence(sf::Vector2f position) : m_amount(100.0f) {
    m_shape.setRadius(8.f);
    m_shape.setFillColor(sf::Color(255, 255, 100)); // Glowing Yellow
    m_shape.setOrigin({8.f, 8.f});
    m_shape.setPosition(position);
}

bool SpiritEssence::drain(float amount) {
    m_amount -= amount;
    // Fade out based on remaining amount
    float alpha = (m_amount / 100.0f) * 255;
    sf::Color c = m_shape.getFillColor();
    c.a = static_cast<sf::Uint8>(std::max(0.f, alpha));
    m_shape.setFillColor(c);
    return m_amount <= 0.f;
}

void SpiritEssence::draw(sf::RenderWindow& window) {
    window.draw(m_shape);
}

sf::Vector2f SpiritEssence::getPosition() const {
    return m_shape.getPosition();
}
