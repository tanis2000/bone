//
//  Binocle
//  Copyright(C)2015-2017 Valerio Santinelli
//

#ifndef MyGame_hpp
#define MyGame_hpp

#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>
#include <Binocle/Core/Game.hpp>
#include <Binocle/Core/SpriteBatch.hpp>
#include <Binocle/Core/Texture.hpp>
#include <Binocle/Core/Camera.hpp>
#include <Binocle/Core/EverythingRenderer.hpp>
#include <Binocle/Core/Entity.hpp>
#include <Binocle/Core/Atlas.hpp>
#include <Binocle/Core/TileMap.hpp>
#include <Binocle/Core/SpatialHash.hpp>
#include <Binocle/Core/Audio.hpp>
#include <Binocle/Core/SpriteComponent.hpp>
#include <Binocle/Core/TagRenderer.hpp>
#include <Binocle/Core/TagExcludeRenderer.hpp>
#include <Binocle/Core/ParticleSystem.hpp>
#include <Binocle/Core/ParticleType.hpp>
#include <Binocle/Core/ScalingViewportAdapter.hpp>
#include <Binocle/Core/BitmapFont.hpp>
#include <Binocle/Core/FPS.hpp>
#include <Binocle/Core/Gui.hpp>
#include "sys_config.h"

using namespace Binocle;

namespace UberGame {
    class Bunny;
    class Bullet;
    
    enum MyTags {
        Everything = 0,
        GUI = 1,
        Movables = 2
    };
    
    enum MyLayers {
        All = -1,
        Bunnies = 1 << 0,
        Platforms = 1 << 1
    };
    
    class MyGame : public Game {
    public:
        
        MyGame(Window *window);
        virtual ~MyGame();
        
        void Update(float dt);
        void Draw(float dt);
        //float GetFps(const sf::Time& time);
        void testAnimate(SpriteComponent *sc);
        void BuildGui();
        
        
        bool showTiled = false;
        bool showOverlap2D = false;
        bool showBunnies = false;
        bool showLabels = true;
        bool showTweenBunny = false;
        bool showLibGdxAnimation = true;
        bool showParticles = false;
      int spatialHashCellSize = 32; // 200
      
        SpriteBatch *spriteBatch;
        Texture *wabbit;
        std::vector<Bunny*> bunnies;
        
        Camera miniMapCamera;
        Camera playerCamera;
        Camera guiCamera;
        ScalingViewportAdapter *miniMapVA;
        ScalingViewportAdapter *playerVA;
        ScalingViewportAdapter *guiVA;
        
        std::vector<Camera>cameras;
        EverythingRenderer *miniMapRenderer;
        TagExcludeRenderer *playerRenderer;
        TagRenderer *guiRenderer;
        float cameraZoom = 0;
        float cameraAngle = 0;
        float cameraZoomInc = 0.1f;
        float cameraAngleInc = 0.10f;
        BitmapFont *defaultFont;
        SpriteFont *defaultSpriteFont;
        BitmapFont *commandFont;
        SpriteFont *commandsSpriteFont;
        Entity *fps;
        Entity *hero;
        Atlas *atlas;
        Atlas *libGdxAtlas;
        FPSCounter *fpsC;
        TileMap *tileMap;
        Scene *mainScene;
        Music *music;
        Entity *loopingBunny;
        float tweenTime = 0;
        ParticleSystem *particleSystem;
        ParticleType pt;
        std::vector<Bullet *>bullets;
        std::vector<Entity *>labels;

        // ImGui stuff
        bool show_test_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImColor(114, 144, 154);
    };
}

#endif /* MyGame_hpp */