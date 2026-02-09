#ifndef RIVAL_HPP
#define RIVAL_HPP
#include <Entities/Entity.hpp>

enum class RivalState {
    Wandering,
    Meditating,
    Fleeing
};

class Rival : public Entity {
public:
    Rival(sf::Vector2f position);
    void update(float dt, sf::Vector2f mapBounds, sf::Vector2f playerPos, int playerPower);
    bool isMeditating() const override { return m_state == RivalState::Meditating; }

private:
    RivalState m_state;
    float m_stateTimer;
};
#endif