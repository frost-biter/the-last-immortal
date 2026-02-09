#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <SFML/Graphics.hpp>
#include <Entities/Entity.hpp>

class Player : public Entity {
public:
    Player();
    void handleInput();
    void update(float dt, sf::Vector2f mapBounds);
    bool isMeditating() const override { return m_isMeditating; }
    bool hasBreakthrough();
    void draw(sf::RenderWindow& window);
    void drawUI(sf::RenderWindow& window);
    bool checkBlast(); // Returns true if blast triggered

private:
    void updateRealmText();

    sf::Sprite m_lightningSprite;
    
    // Animation State
    float m_animTimer;
    int m_currentFrame;
    float m_lightningAnimTimer;
    int m_lightningFrame;

    bool m_isMeditating;
    float qiProgress = 0.f;
    float trainingSpeed = 0.002f;
    bool m_breakthroughTriggered = false;
    sf::RectangleShape m_qiBarBack;
    sf::RectangleShape m_qiBarFill;
    
    sf::Text m_realmText;
    
    bool m_wantsToBlast;
    float m_blastCooldown;
    int m_karma;
};
#endif