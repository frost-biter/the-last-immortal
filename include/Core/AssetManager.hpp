#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>

struct Animation {
    std::string textureName;
    std::vector<sf::IntRect> frames;
    float frameTime;
    bool looping;
};

class AssetManager {
public:
    static AssetManager& instance();
    
    const Animation& getAnimation(const std::string& name);
    void loadAnimations();

private:
    AssetManager();
    std::map<std::string, Animation> m_animations;
    
    void addAnimation(const std::string& name, const std::string& texture, int startId, int frameCount, int tileWidth, int tileHeight, int columns, float duration);
};

#endif