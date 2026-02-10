#include <Entities/SpiritEssence.hpp>
#include <Core/AssetManager.hpp>
#include <Core/ResourceManager.hpp>

SpiritEssence::SpiritEssence(sf::Vector2f position) : m_amount(100.0f), m_frame(0), m_timer(0.f) {
    m_anim = &AssetManager::instance().getAnimation("SpiritEssence");
    
    m_sprite.setTexture(ResourceManager::instance().getTexture(m_anim->textureName));
    if (!m_anim->frames.empty()) {
        m_sprite.setTextureRect(m_anim->frames[0]);
        m_sprite.setOrigin(m_anim->frames[0].width / 2.f, m_anim->frames[0].height / 2.f);
    }
    m_sprite.setPosition(position);
    m_sprite.setScale(0.8f, 0.8f); // Adjust scale to fit tile
}

void SpiritEssence::update(float dt) {
    if (!m_anim) return;
    m_timer += dt;
    if (m_timer >= m_anim->frameTime) {
        m_timer = 0.f;
        m_frame = (m_frame + 1) % m_anim->frames.size();
        m_sprite.setTextureRect(m_anim->frames[m_frame]);
    }
}

bool SpiritEssence::drain(float amount) {
    m_amount -= amount;
    // Fade out based on remaining amount
    float alpha = (m_amount / 100.0f) * 255;
    m_sprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(std::max(0.f, alpha))));
    return m_amount <= 0.f;
}

void SpiritEssence::draw(sf::RenderWindow& window) {
    window.draw(m_sprite);
}

sf::Vector2f SpiritEssence::getPosition() const {
    return m_sprite.getPosition();
}
