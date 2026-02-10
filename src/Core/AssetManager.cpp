#include <Core/AssetManager.hpp>
#include <stdexcept>

AssetManager::AssetManager() {
    loadAnimations();
}

AssetManager& AssetManager::instance() {
    static AssetManager inst;
    return inst;
}

const Animation& AssetManager::getAnimation(const std::string& name) {
    if (m_animations.find(name) == m_animations.end()) {
        // Return a dummy or throw, for now let's assume it exists or return empty
        static Animation empty;
        return empty;
    }
    return m_animations[name];
}

void AssetManager::addAnimation(const std::string& name, const std::string& texture, int startId, int frameCount, int tileWidth, int tileHeight, int columns, float duration) {
    Animation anim;
    anim.textureName = texture;
    anim.frameTime = duration;
    anim.looping = true;

    for (int i = 0; i < frameCount; ++i) {
        int id = startId + i;
        int col = id % columns;
        int row = id / columns;
        anim.frames.emplace_back(col * tileWidth, row * tileHeight, tileWidth, tileHeight);
    }
    m_animations[name] = anim;
}

void AssetManager::loadAnimations() {
    // --- CULTIVATOR ASSETS ---
    {
        std::string tex = "assets/character/cultivator_sprites_transparent.png";
        int w = 352;
        int h = 384;
        int cols = 8;
        int len = 8;

        addAnimation("Cultivator_Standing", tex, 0,  len, w, h, cols, 0.15f);
        addAnimation("Cultivator_Walking",  tex, 8,  len, w, h, cols, 0.15f);
        addAnimation("Cultivator_Sitting",  tex, 16, len, w, h, cols, 0.15f);
        addAnimation("Cultivator_Attacking",tex, 24, len, w, h, cols, 0.1f);
    }

    // --- RIVAL ASSETS ---
    {
        std::string tex = "assets/character/rival_sprites_transparent.png"; 
        int w = 352;
        int h = 384;
        int cols = 8;
        int len = 8;

        addAnimation("Rival_Standing", tex, 0,  len, w, h, cols, 0.15f);
        addAnimation("Rival_Walking",  tex, 8,  len, w, h, cols, 0.15f);
        addAnimation("Rival_Sitting",  tex, 16, len, w, h, cols, 0.15f);
        addAnimation("Rival_Attacking",tex, 24, len, w, h, cols, 0.1f);
    }

    // Spirit Essence (Nebula)
    // 800x800, 8x8, 61 frames
    std::string eTex = "assets/Free Pixel Effects Pack/12_nebula_spritesheet.png";
    addAnimation("SpiritEssence", eTex, 0, 61, 100, 100, 8, 0.05f);
}