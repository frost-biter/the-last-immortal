#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

class ResourceManager {
private:
    std::map<std::string, sf::Font> m_fonts;
    std::map<std::string, sf::Texture> m_textures;
    // We make it a Singleton so only one exists
    ResourceManager() {} 
    ResourceManager(const ResourceManager&) = delete;
    void operator=(const ResourceManager&) = delete;

public:
    static ResourceManager& instance() {
        static ResourceManager inst;
        return inst;
    }

    sf::Font& getFont(const std::string& filename) {
        if (m_fonts.find(filename) == m_fonts.end()) {
            m_fonts[filename].loadFromFile(filename); // Load only if not already in map
        }
        return m_fonts[filename];
    }

    sf::Texture& getTexture(const std::string& filename) {
        if (m_textures.find(filename) == m_textures.end()) {
            sf::Image img;
            if (img.loadFromFile(filename)) {
                img.createMaskFromColor(sf::Color(248, 4, 246));
                m_textures[filename].loadFromImage(img);
            }
        }
        return m_textures[filename];
    }
};
#endif