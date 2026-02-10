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
    // -----------------------------------------------------------
    // FIX: Using calculated dimensions from 2816x1536 image
    // Frame Width: 352px, Frame Height: 384px
    // -----------------------------------------------------------

    int frameW = 352; 
    int frameH = 384;
    int cols = 8;
    // Use all 8 frames for smooth animation
    int animLen = 8; 

    // CULTIVATOR (Blue)
    std::string pTex = "assets/character/cultivator_sprites.png"; 
    
    // Row 0 (ID 0) -> Standing
    addAnimation("Cultivator_Standing", pTex, 0,  animLen, frameW, frameH, cols, 0.15f);
    // Row 1 (ID 8) -> Walking
    addAnimation("Cultivator_Walking",  pTex, 8,  animLen, frameW, frameH, cols, 0.15f);
    // Row 2 (ID 16) -> Sitting
    addAnimation("Cultivator_Sitting",  pTex, 16, animLen, frameW, frameH, cols, 0.15f);
    // Row 3 (ID 24) -> Attacking
    addAnimation("Cultivator_Attacking",pTex, 24, animLen, frameW, frameH, cols, 0.1f);

    // RIVAL (Red)
    std::string rTex = "assets/character/rival_sprites.png"; 
    int rivalW = 176; // 1408 / 8
    int rivalH = 188; // 752 / 4

    // Uses same layout
    addAnimation("Rival_Standing", rTex, 0,  animLen, rivalW, rivalH, cols, 0.15f);
    addAnimation("Rival_Walking",  rTex, 8,  animLen, rivalW, rivalH, cols, 0.15f);
    addAnimation("Rival_Sitting",  rTex, 16, animLen, rivalW, rivalH, cols, 0.15f);
    addAnimation("Rival_Attacking",rTex, 24, animLen, rivalW, rivalH, cols, 0.1f);

    // Spirit Essence (Nebula)
    // 800x800, 8x8, 61 frames
    std::string eTex = "assets/Free Pixel Effects Pack/12_nebula_spritesheet.png";
    addAnimation("SpiritEssence", eTex, 0, 61, 100, 100, 8, 0.05f);
}