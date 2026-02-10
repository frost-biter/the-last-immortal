#include <Entities/Entity.hpp>
#include <Core/ResourceManager.hpp>
#include <Core/AssetManager.hpp>

Entity::Entity(float radius, sf::Color color, sf::Vector2f position, float speed) 
    : m_speed(speed), m_realm(Realm::QiCondensation), m_layer(1), m_currentQi(0.f), m_maxQi(100.f), m_name("Unknown"), m_knockback(0.f, 0.f) {
    m_shape.setRadius(radius);
    m_shape.setFillColor(color);
    m_shape.setOrigin({radius, radius});
    m_shape.setPosition(position);
    m_sprite.setScale(0.125f, 0.125f);
}

void Entity::draw(sf::RenderWindow& window) {
    if (m_sprite.getTexture()) {
        window.draw(m_sprite);
    } else {
        window.draw(m_shape);
    }
}

sf::Vector2f Entity::getPosition() const {
    return m_shape.getPosition();
}

void Entity::setTexture(const std::string& path) {
    m_sprite.setTexture(ResourceManager::instance().getTexture(path));
    // Default origin to center-bottom of the sprite, assuming it stands on the point
    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.width / 2.f, bounds.height);
}

void Entity::playAnimation(const std::string& name) {
    const Animation& anim = AssetManager::instance().getAnimation(name);
    if (m_currentAnim == &anim) return; // Already playing

    m_currentAnim = &anim;
    m_animFrame = 0;
    m_animTimer = 0.f;
    
    // Set texture and initial frame
    setTexture(anim.textureName);
    if (!anim.frames.empty()) {
        m_sprite.setTextureRect(anim.frames[0]);
        // Re-center origin based on new frame size
        m_sprite.setOrigin(anim.frames[0].width / 2.f, static_cast<float>(anim.frames[0].height));
    }
}

void Entity::updateAnimation(float dt) {
    if (!m_currentAnim || m_currentAnim->frames.empty()) return;

    m_animTimer += dt;
    if (m_animTimer >= m_currentAnim->frameTime) {
        m_animTimer = 0.f;
        m_animFrame++;
        
        if (m_animFrame >= m_currentAnim->frames.size()) {
            if (m_currentAnim->looping) m_animFrame = 0;
            else m_animFrame = m_currentAnim->frames.size() - 1;
        }

        m_sprite.setTextureRect(m_currentAnim->frames[m_animFrame]);
    }
}

void Entity::setPosition(sf::Vector2f pos) {
    m_shape.setPosition(pos);
    m_sprite.setPosition(pos);
}

void Entity::moveAndClamp(float dt, sf::Vector2f mapBounds) {
    // Apply Knockback decay (friction)
    m_knockback *= 0.9f;
    if (std::abs(m_knockback.x) < 1.f && std::abs(m_knockback.y) < 1.f) m_knockback = {0.f, 0.f};

    // Combine self-propulsion (velocity) and external force (knockback)
    sf::Vector2f totalVelocity = m_velocity + m_knockback;

    // Flip sprite based on movement direction (Mirroring)
    if (totalVelocity.x < -0.1f) {
        m_sprite.setScale(-0.125f, 0.125f);
    } else if (totalVelocity.x > 0.1f) {
        m_sprite.setScale(0.125f, 0.125f);
    }

    sf::Vector2f nextPos = m_shape.getPosition() + totalVelocity * dt;
    float radius = m_shape.getRadius();

    // Basic movement without clamping (clamping is done via WorldMap::clampEntity)
    // We ignore mapBounds here for isometric logic
    // if (nextPos.x - radius < 0.f) nextPos.x = radius;
    // ...

    m_shape.setPosition(nextPos);
    m_sprite.setPosition(nextPos);
}

void Entity::applyKnockback(sf::Vector2f force) {
    m_knockback += force;
}

void Entity::modifyQi(float amount) {
    m_currentQi += amount;
    if (m_currentQi > m_maxQi) m_currentQi = m_maxQi;
    if (m_currentQi < 0.f) m_currentQi = 0.f;
}