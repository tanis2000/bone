//
//  Binocle
//  Copyright(C)2015-2017 Valerio Santinelli
//

#include "MyGame.hpp"
#include <math.h>
#include <Binocle/Core/InputManager.hpp>
#include <Binocle/Core/SpriteBatch.hpp>
#include <Binocle/Core/Random.hpp>
#include <Binocle/Core/Log.hpp>
#include <Binocle/Core/Entity.hpp>
#include <Binocle/Core/Scene.hpp>
#include <Binocle/Core/EverythingRenderer.hpp>
#include <Binocle/Core/ImageComponent.hpp>
#include <Binocle/Core/TextComponent.hpp>
#include <Binocle/Core/Atlas.hpp>
#include <Binocle/Core/SpriteComponent.hpp>
#include <Tmx.h>
#include <Binocle/Core/TileComponent.hpp>
#include <Binocle/Core/Circle.hpp>
#include <Binocle/Core/Collide.hpp>
#include <Binocle/Core/Hitbox.hpp>
#include <Binocle/Core/Draw.hpp>
#include <Binocle/Core/Audio.hpp>
#include <Binocle/Core/SpritesheetComponent.hpp>
#include <Overlap2D.hpp>
#include <Binocle/Core/ParticleType.hpp>
#include <Binocle/Core/SAT2D.hpp>
#include <Binocle/Core/CollisionData.hpp>
#include <Binocle/Core/Macros.hpp>
#include <Binocle/Core/Utils.hpp>
#include <Binocle/Core/ScriptManager.hpp>
#include <imgui.h>


namespace UberGame {

    class Bunny : public Entity {
    public:
        float gravity = 10.0f;//0.5f;
        float speedX;
        float speedY;
        float posX;
        float posY;
        int minX;
        int minY;
        int maxX;
        int maxY;
        std::vector<Colliders::Collider *> *colls;

        Bunny():Entity() {
            visible = true;
            active = true;
            collidable = true;
            posX = Random::Int(0, 100);
            posY = Random::Int(0, 100);

            speedX = Random::Float(0, 1) * 500;
            speedY = ( Random::Float(0, 1) * 50 ) - 25;
            minX = 0;
            minY = 0;
            maxX = 640;
            maxY = 480;

            colls = new std::vector<Colliders::Collider *>();
        }

        void Update()
        {
            bool collides = false;
            Entity::Update();
            posX += speedX * Game::deltaTime;
            posY += speedY * Game::deltaTime;
            speedY += gravity;

            if( posX > maxX )
            {
                speedX *= -1;
                posX = maxX;
            }
            else if( posX < minX )
            {
                speedX *= -1;
                posX = minX;
            }

            if( posY > maxY )
            {
                speedY *= -0.8;
                posY = maxY;
                if( Random::Float(0, 1) > 0.5 )
                    speedY -= 600 + Random::Float(0, 1) * 40;
            }
            else if( posY < minY )
            {
                speedY = 0;
                posY = minY;
            }

            // rotation
            //bunny.rotZ = luxe.utils.Maths.wrap_angle( bunny.rotZ + 360 * dt, 0, 360 );
            position.x = posX;
            position.y = posY;
            /*if (scene != NULL) {
                for (auto const& e : *scene->entities.GetEntities()) {
                    if (this->CollideCheck(e)) {
                        float dist = sqrt( (e->position.x-position.x)*(e->position.x-position.x) + (e->position.y-position.y)*(e->position.y-position.y) );
                        float ang = atan2(e->position.y-position.y, e->position.x-position.x);
                        float force = 0.2f;
                        float repelPower = 20.0f;//(radius+e.radius - dist) / (radius+e.radius);
                        posX -= cos(ang) * repelPower * force;
                        posY -= sin(ang) * repelPower * force;
                        e->position.x += cos(ang) * repelPower * force;
                        e->position.y += sin(ang) * repelPower * force;
                        speedX *= -1;
                        speedY *= -1;
                        position.x = posX;
                        position.y = posY;

                        //Log::log("collisione");
                    }
                }
            }*/

            if (scene != NULL) {
                for (auto const& c : *colliders.GetColliders()) {
                    colls->clear();
                    this->scene->spatialHash->GetAllBodiesSharingCellsWithBody(c, colls, MyLayers::All);
                    for (auto const otherColl : *colls) {
                        if (c->Collide(otherColl)) {
                            collides = true;
                            Colliders::Collider *other = colls->at(0);
                            Entity *e = other->entity;
                            float dist = sqrt( (e->position.x-position.x)*(e->position.x-position.x) + (e->position.y-position.y)*(e->position.y-position.y) );
                            float ang = atan2(e->position.y-position.y, e->position.x-position.x);
                            float force = 0.2f;
                            float repelPower = 20.0f;//(radius+e.radius - dist) / (radius+e.radius);
                            posX -= cos(ang) * repelPower * force;
                            posY -= sin(ang) * repelPower * force;
                            e->position.x += cos(ang) * repelPower * force;
                            e->position.y += sin(ang) * repelPower * force;
                            speedX *= -1;
                            speedY *= -1;
                            position.x = posX;
                            position.y = posY;
                            break;
                        }
                    }
                }
            }


            //UpdateAABB();
        }

        void UpdateAABB()
        {
            /*
            body->x = position.x;
            body->y = position.y;
            body->aabb.min.x = position.x;
            body->aabb.min.y = position.y;
            body->aabb.max.x = position.x + 26;
            body->aabb.max.y = position.y + 37;
             */
        }


    };

    class Bullet : public Entity
    {
    public:
        float speedX;
        float speedY;
        float posX;
        float posY;
        std::vector<Colliders::Collider *> *colls;
        Colliders::CollisionData *collisionData;
        Colliders::Hitbox *myColl;

        Bullet():Entity() {
            visible = true;
            active = true;
            collidable = true;
            posX = 0;
            posY = 0;

            speedX = 0;
            speedY = 4000;
            colls = new std::vector<Colliders::Collider *>();
            collisionData = new Colliders::CollisionData();

            myColl = new Colliders::Hitbox(16, 16);
            myColl->position.x = -8;
            myColl->position.y = -8;
            Add(myColl);

        }

        void Update()
        {
            bool collides = false;
            Entity::Update();

            if (scene != NULL) {
                for (auto const& c : *colliders.GetColliders()) {
                    colls->clear();
                    this->scene->spatialHash->GetAllBodiesSharingCellsWithBody(c, colls, MyLayers::Platforms);
                    for (auto const otherColl : *colls) {
                        Colliders::Hitbox *hb = dynamic_cast<Colliders::Hitbox *>(otherColl);
                        Rectangle r1 = Rectangle(myColl->GetAbsoluteX(), myColl->GetAbsoluteY(), myColl->GetWidth(), myColl->GetHeight());
                        Rectangle r2 = Rectangle(hb->GetAbsoluteX(), hb->GetAbsoluteY(), hb->GetWidth(), hb->GetHeight());
                        float normalX = 0;
                        float normalY = 0;
                        float collPosition = 0;
                        float dt = Game::deltaTime;
                        //collPosition = Collide::SweptAABB(r1, r2, speedX * Game::deltaTime, speedY * Game::deltaTime, normalX, normalY);
                        glm::vec2 speed = glm::vec2(speedX * Game::deltaTime, speedY * Game::deltaTime);
                        glm::vec2 normals = glm::vec2(0, 0);
                        glm::vec2 impactSpeed = glm::vec2(0, 0);
                        Rectangle broad = Collide::GetSweptBroadphaseRect(r1, speed);
                        /*if (collPosition < 1.0f) {
                            collides = true;
                            posY += (speedY * collPosition * Game::deltaTime);
                            speedY = 0;
                            break;
                        }*/
                        if (speedY == 0) break;
                        if (broad.overlaps(r2)) {
                            if (Collide::SweptAABB(r1, r2, speed, impactSpeed, normals)) {
                                collides = true;
                                posY += impactSpeed.y;
                                speedY = 0;
                                break;
                            }
                        }
                        /*if (SAT2D::TestRectVsRect(*myColl, *hb, *collisionData)) {
                            collides = true;
                            Colliders::Collider *other = collisionData->collider2;
                            Entity *e = other->entity;
                            float dist = sqrt( (e->position.x-position.x)*(e->position.x-position.x) + (e->position.y-position.y)*(e->position.y-position.y) );
                            float ang = atan2(e->position.y-position.y, e->position.x-position.x);
                            float force = 0.2f;
                            float repelPower = 20.0f;//(radius+e.radius - dist) / (radius+e.radius);
                            //posX -= cos(ang) * repelPower * force;
                            //posY -= sin(ang) * repelPower * force;
                            //e->position.x += cos(ang) * repelPower * force;
                            //e->position.y += sin(ang) * repelPower * force;
                            //speedX *= -1;
                            //speedY *= -1;
                            if (collisionData->overlap > 0) {
                                posX = posX + collisionData->separation.x;
                                posY = posY + collisionData->separation.y;
                            }
                            speedY = 0;
                            position.x = posX;
                            position.y = posY;
                            break;
                        }*/
                    }
                }
            }
            posX += speedX * Game::deltaTime;
            posY += speedY * Game::deltaTime;
            position.x = (int)posX;
            position.y = (int)posY;

        }

    };

    class RotateComponent : public Component
    {
    public:
        float angle = 0;

        RotateComponent():Component(true, false) {

        }
        void Update() {
            Component::Update();
            angle += 0.2f * Game::deltaTime;
            if (angle > 2 * M_PI) angle = 0;
            SpriteComponent *sc = this->entity->GetComponent<SpriteComponent>();
            if (sc != NULL) {
                sc->rotation = angle;
            }
        }
    };

    MyGame::MyGame(Window *window):
    Game(window)
    {
        GetScriptManager()->Init(std::string(BINOCLE_DATA_DIR), std::string(BINOCLE_DATA_DIR) + "main.lua");
        window->setBackgroundColor(Color(0, 0.7f, 0.7f, 1.0f));

        miniMapVA = new ScalingViewportAdapter(window, 320, 240);
      miniMapVA->SetViewport(Rectangle(320-75, 0, 75, 50));
        miniMapCamera.SetViewportAdapter(miniMapVA);
        //miniMapCamera.cameraResizeType = CameraResizeTypeProportional;
        //miniMapCamera.SetCenter(160, 120);
        //miniMapCamera.SetSize(75, 50);
        //miniMapCamera.SetViewport(Rectangle((320.0f-75.0f), 0, 75.0f, 50.0f));
        //miniMapCamera.HandleWindowResize(Vector2(320, 240), Vector2(window->width, window->height));

        playerVA = new ScalingViewportAdapter(window, 320, 240);
        playerCamera.SetViewportAdapter(playerVA);
        //playerCamera.SetCenter(160, 120);
        //playerCamera.SetSize(320, 240);
        //playerCamera.SetViewport(Rectangle(0, 0, 320, 240));
        //playerCamera.cameraResizeType = CameraResizeTypePixelPerfect;
        //playerCamera.HandleWindowResize(Vector2(320, 240), Vector2(window->width, window->height));

        guiVA = new ScalingViewportAdapter(window, 320, 240);
        guiCamera.SetViewportAdapter(guiVA);
        //guiCamera.SetCenter(160, 120);
        //guiCamera.SetSize(320, 240);
        //guiCamera.SetViewport(Rectangle(0, 0, 320, 240));
        //guiCamera.HandleWindowResize(Vector2(320, 240), Vector2(window->width, window->height));
        cameras.push_back(playerCamera);
        cameras.push_back(miniMapCamera);
        cameras.push_back(guiCamera);

        mainScene = new Scene(spatialHashCellSize);
        miniMapRenderer = new EverythingRenderer(window, &miniMapCamera);
        miniMapRenderer->camera = &miniMapCamera;
        playerRenderer = new TagExcludeRenderer(window, &playerCamera, MyTags::GUI);
        playerRenderer->camera = &playerCamera;
        guiRenderer = new TagRenderer(window, &guiCamera, MyTags::GUI);
        guiRenderer->camera = &guiCamera;

        mainScene->renderers.push_back(playerRenderer);
        mainScene->renderers.push_back(miniMapRenderer);
        mainScene->renderers.push_back(guiRenderer);

        nextScene = mainScene;

        wabbit = new Texture();
        if (!wabbit->loadFromFile(std::string(BINOCLE_DATA_DIR) + "wabbit_alpha.png")) {
            Log::error("Image not found.");
        }

      defaultFont = new BitmapFont(std::string(BINOCLE_DATA_DIR) + "font.fnt");
      defaultFont->LoadTexture(std::string(BINOCLE_DATA_DIR) + "font.png");


      defaultSpriteFont = new SpriteFont();
        defaultSpriteFont->SetFont(*defaultFont);
        defaultSpriteFont->SetCharacterSize(32);

        // TODO: For some reason you can't reuse the same BitmapFont. Gotta look at it.
        commandFont = new BitmapFont(std::string(BINOCLE_DATA_DIR) + "font.fnt");
        commandFont->LoadTexture(std::string(BINOCLE_DATA_DIR) + "font.png");

        commandsSpriteFont = new SpriteFont();
        commandsSpriteFont->SetFont(*commandFont);
        commandsSpriteFont->SetCharacterSize(8);

        Draw::SetDefaultFont(defaultSpriteFont);

        atlas = new Atlas(std::string(BINOCLE_DATA_DIR) + "testatlas.json", std::string(BINOCLE_DATA_DIR) + "testatlas.png", window->renderer, true);

        libGdxAtlas = new Atlas(std::string(BINOCLE_DATA_DIR) + "pack.atlas", std::string(BINOCLE_DATA_DIR) + "pack.png", window->renderer, true, Binocle::Atlas::LibGdx);

        tileMap = new TileMap(window->renderer);
        if (!tileMap->LoadFromFile(std::string(BINOCLE_DATA_DIR) + "tilemap.tmx", std::string(BINOCLE_DATA_DIR))) {
            Log::error("Error loading TMX file");
        }

        if (showTiled) {
            for (uint32_t i = 0; i < tileMap->tileLayers.size() ; i++) {
                TileLayer *layer = &tileMap->tileLayers[i];
                for (int y = 0; y < layer->height; y++)
                {
                    for (int x = 0; x < layer->width; x++)
                    {
                        Tile *t = &layer->tiles[y*layer->width+x];
                        if (t == nullptr || t == NULL || t->gid == 0)
                        {
                            continue;
                        }
                        Entity *e = new Entity();
                        e->Tag(MyTags::Movables);
                        e->visible = true;
                        e->active = true;
                        e->position.x = x * t->size.x;
                        e->position.y = y * t->size.y;
                        TileComponent *tc = new TileComponent(t);
                        tc->origin.x = tc->tile->size.x / 2.0f;
                        tc->origin.y = tc->tile->size.y / 2.0f;
                        e->Add(tc);
                        if (i == 1) {
                            Colliders::Hitbox *c = new Colliders::Hitbox(t->size.x, t->size.y, -tc->tile->size.x / 2.0f, -tc->tile->size.y / 2.0f);
                            c->layer = MyLayers::Platforms;
                            e->Add(c);
                        }
                        mainScene->Add(e);
                        e->SetDepth(i);
                    }
                }
            }
        }

        /*
        sf::FileInputStream stream;
        if (!stream.open(std::string(BINOCLE_DATA_DIR) + "MainScene.dt")) {
            throw "Cannot open JSON scene file";
        }

        std::string content;
        content.resize(stream.getSize());
        stream.seek(0);
        stream.read(&content[0], content.size());

        Overlap2D::O2DScene *o2ds = new Overlap2D::O2DScene();
        o2ds->Parse(content);

        if (showOverlap2D) {
            for (auto si : o2ds->composite->sImages) {
                Entity *e = new Entity();
                e->visible = true;
                e->active = true;
                e->Tag(MyTags::Movables);
                e->SetDepth(5 + si->zIndex);
                e->position.x = si->x;
                e->position.y = si->y;
                SpriteComponent *sc = new SpriteComponent();
                sc->AddFrame(&libGdxAtlas->subtextures[si->imageName]);
                sc->origin.x = si->originX;
                sc->origin.y = si->originY;
                sc->scale.x = si->scaleX;
                sc->scale.y = si->scaleY;
                e->Add(sc);
                mainScene->Add(e);
            }
        }*/

        if (showBunnies) {
            for (int i = 0 ; i < 10 ; i++) {
                Bunny *b = new Bunny();
                b->Tag(MyTags::Movables);
                Rectangle wabbitRect = Rectangle(0, 0, wabbit->getWidth(), wabbit->getHeight());
                ImageComponent *ic = new ImageComponent(wabbit, &wabbitRect, true);
                b->Add(ic);
                //Colliders::Circle *c = new Colliders::Circle(wabbit->getSize().x / 2);
                Colliders::Hitbox *c = new Colliders::Hitbox(wabbit->getWidth(), wabbit->getHeight(), 0, 0);
                c->layer = MyLayers::Bunnies;
                //c->position.x = wabbit->getSize().x/2;
                //c->position.y = wabbit->getSize().y/2;
                b->Add(c);
                mainScene->Add(b);
                b->SetDepth(10);
                bunnies.push_back(b);
            }
        }

        fps = new Entity();
        fps->Tag(MyTags::GUI);
        fps->active = true;
        fps->visible = true;
        TextComponent *tc = new TextComponent();
        tc->spriteFont = defaultSpriteFont;
        tc->text = "FPS: ";
        tc->position.x = 0;
        tc->position.y = 0;
        fps->Add(tc);
        mainScene->Add(fps);
        fps->SetDepth(50);

        hero = new Entity();
        hero->Tag(MyTags::Movables);
        hero->active = true;
        hero->visible = true;
        SpriteComponent *sc = new SpriteComponent();
        sc->AddFrame(&atlas->subtextures["player.png"]);
        sc->AddFrame(&atlas->subtextures["enemy.png"]);
        sc->AddFrame(&atlas->subtextures["friend.png"]);
      std::vector<int> anim;
      anim.push_back(0);
      anim.push_back(1);
      anim.push_back(2);
        sc->AddAnimation(0, true, 1, anim);
        // Center the origin
        sc->origin.x = sc->frames[0].subtexture->GetSize().x / 2.0f;
        sc->origin.y = sc->frames[0].subtexture->GetSize().y / 2.0f;
        //sc->OnAnimationStarted = [&](SpriteComponent *c) { /*Log::log("Animation started");*/};
        //sc->OnAnimate = [&](SpriteComponent *c) { /*Log::log("Sprite is animating");*/};
        //sc->OnFrameChange = [&](SpriteComponent *c) { /*Log::log("Frame changed");*/};
        //sc->OnAnimationComplete = [&](SpriteComponent *c) { /*Log::log("Animation finished");*/};
        sc->Play(0);
        hero->Add(sc);
        /*Rectangle heroRect = Rectangle(0, 0, wabbit->getSize().x, wabbit->getSize().y);
        ImageComponent *ic = new ImageComponent(wabbit, &heroRect, true);
        hero->Add(ic);*/
        /*SpritesheetComponent *ssc = new SpritesheetComponent(&atlas->texture, 16, 16);
        ssc->origin.x = 8;
        ssc->origin.y = 8;
        ssc->color = sf::Color::White;
        std::vector<int>frames = {0, 1, 2};
        ssc->Add(0, 0, true, &frames);
        ssc->Play(0);
        hero->Add(ssc);*/
        RotateComponent *rc = new RotateComponent();
        hero->Add(rc);
        mainScene->Add(hero);

        //hero->position.x = 160;
        //hero->position.y = 130;

        hero->SetDepth(20);
        // Begin adds a test collider to the hero
        Colliders::Hitbox *myColl = new Colliders::Hitbox(16, 16);
        myColl->position.x = -8;
        myColl->position.y = -8;
        hero->Add(myColl);
        // End of hero collider stuff


        fpsC = new Binocle::FPSCounter();

        music = Audio::GetInstance()->LoadMusic(std::string(BINOCLE_DATA_DIR) + "8bit.ogg");
        Audio::GetInstance()->PlayMusic(music, true);

        if (showTweenBunny) {
            loopingBunny = new Entity();
            loopingBunny->visible = true;
            loopingBunny->active = true;
            loopingBunny->position.x = 150;
            loopingBunny->position.y = 170;
            Rectangle loopingBunnyRect = Rectangle(0, 0, wabbit->getWidth(), wabbit->getHeight());
            ImageComponent *iclb = new ImageComponent(wabbit, &loopingBunnyRect, true);
            loopingBunny->Add(iclb);
            loopingBunny->SetDepth(20);
            mainScene->Add(loopingBunny);
        }

        if (showLibGdxAnimation)
        {
            Entity *e = new Entity();
            e->visible = true;
            e->active = true;
            e->Tag(MyTags::Movables);
            e->SetDepth(20);
            e->position.x = 200;
            e->position.y = 100;
            SpriteComponent *sc = new SpriteComponent();
            std::vector<int> frames;
            for (int i = 0 ; i < libGdxAtlas->subtextures.size() ; i++) {
                sc->AddFrame(&libGdxAtlas->subtextures[String::intToString(i+1)]);
                sc->origin.x = sc->frames[i].subtexture->GetSize().x / 2.0f;
                sc->origin.y = sc->frames[i].subtexture->GetSize().y / 2.0f;
                frames.push_back(i);
            }
            sc->AddAnimation(0, true, 1, frames);
            e->Add(sc);
            mainScene->Add(e);
            sc->Play(0);
        }

        /*if (showParticles) {
            particleSystem = new ParticleSystem(100, 1000);
            particleSystem->visible = true;
            particleSystem->active = true;
            particleSystem->Tag(MyTags::Movables);
            particleSystem->position.x = 50;
            particleSystem->position.y = 50;
            mainScene->Add(particleSystem);
            pt = ParticleType();
            pt.color = Color(255, 0, 0);
            pt.color2 = Color(127, 127, 0);
            pt.life = 2;
            pt.randomRotate = true;
            pt.size = 4;
            pt.speed = 100.0f;
            //pt.colorSwitch = 1;
            pt.source = NULL;
            pt.directionRange = 0.3f;
            particleSystem->Emit(pt, particleSystem->position);
        }*/

        if (showLabels) {
            {
                Entity *e = new Entity();
                e->Tag(MyTags::GUI);
                e->active = true;
                e->visible = true;
                TextComponent *tc = new TextComponent();
                tc->spriteFont = defaultSpriteFont;
                tc->text = "This is the 1st line ";
                tc->position.x = 0;
                tc->position.y = 50;
                e->Add(tc);
                mainScene->Add(e);
                e->SetDepth(50);
                labels.push_back(e);
            }

            {
                Entity *e = new Entity();
                e->Tag(MyTags::GUI);
                e->active = true;
                e->visible = true;
                TextComponent *tc = new TextComponent();
                tc->spriteFont = commandsSpriteFont;
                tc->text = "Second text.. //";
                tc->position.x = 50;
                tc->position.y = 100;
                e->Add(tc);
                mainScene->Add(e);
                e->SetDepth(50);
                labels.push_back(e);
            }

            {
                Entity *e = new Entity();
                e->Tag(MyTags::GUI);
                e->active = true;
                e->visible = true;
                TextComponent *tc = new TextComponent();
                tc->spriteFont = defaultSpriteFont;
                tc->text = "Even more stuff! >.<";
                tc->position.x = 50;
                tc->position.y = 150;
              tc->rotation = M_PI_2;
                e->Add(tc);
                mainScene->Add(e);
                e->SetDepth(50);
                labels.push_back(e);
            }
        }

    }

    MyGame::~MyGame() {
        //delete(music);
    }

    /*float MyGame::GetFps(const sf::Time& time) {
        return (1000000.0f / time.asMicroseconds());
    }*/

    void MyGame::Update(float dt) {
        Game::Update(dt);
        if (paused) return;
        bool collision = false;
        float moveX = 0;
        float moveY = 0;
        /*for (uint32_t i = 0 ; i < bunnies.size() ; i++) {
            //bunnies[i]->Update(dt);
            Bunny *b = bunnies[i];
            spatialHash->UpdateBody(b->body);
            if( spatialHash->IsBodySharingAnyCell( b->body ) )
            {
                Draw::HollowRect(b->position.x, b->position.y, 27, 36, sf::Color::Green);
            } else {
                Draw::HollowRect(b->position.x, b->position.y, 27, 36, sf::Color::Black);
            }
        }*/
        miniMapRenderer->camera->Rotate(cameraAngle);
        cameraAngle += cameraAngleInc * Game::deltaTime;
        if (fabs(cameraAngle) > 1.0f) {
            cameraAngleInc *= -1;
        }

        if (InputManager::getInstance()->isMouseDown(InputManager::MOUSE_LEFT)) {
            std::vector<Colliders::RayCollisionData *> rayCollisionData;
            glm::vec2 mousePos = InputManager::getInstance()->getMousePosition(&playerCamera);
            Ray2 ray = Ray2(hero->position, glm::vec2(mousePos.x,mousePos.y), false);
            Log::log(String::intToString(InputManager::getInstance()->getMouseX()));
            Log::log(String::intToString(InputManager::getInstance()->getMouseY()));
            scene->RayCast(&ray, &rayCollisionData, MyLayers::All);
            if (rayCollisionData.size() > 0)
            {
                Log::log("hit!");
                Entity *e = new Entity();
                e->active = true;
                e->visible = true;
                SpriteComponent *sc = new SpriteComponent();
                sc->AddFrame(&atlas->subtextures["enemy.png"]);
                sc->AddAnimation(0, true, 1, std::vector<int>{0});
                // Center the origin
                sc->origin.x = sc->frames[0].subtexture->GetSize().x / 2.0f;
                sc->origin.y = sc->frames[0].subtexture->GetSize().y / 2.0f;
                sc->Play(0);
                Colliders::RayCollisionData *nearest = Colliders::RayCollisionData::GetNearestCollider(rayCollisionData);
                e->position.x = ray.GetStart().x + ray.GetDirection().x * nearest->start;
                e->position.y = ray.GetStart().y + ray.GetDirection().y * nearest->start;
                e->SetDepth(200);
                e->Add(sc);
                mainScene->Add(e);

            }


        }

        if (InputManager::getInstance()->isMouseDown(InputManager::MOUSE_RIGHT)) {
            // We remove the rotation component if we right click on the player
            glm::vec2 mousePos = playerCamera.ScreenToWorldPoint(glm::vec2(InputManager::getInstance()->getMouseX(), InputManager::getInstance()->getMouseY()));
            SpriteComponent *sc = hero->GetComponent<SpriteComponent>();
            if (sc != NULL) {
                Rectangle playerRect = Rectangle(hero->position.x - sc->frames[0].subtexture->GetWidth()/2.0f, hero->position.y - sc->frames[0].subtexture->GetHeight()/2.0f, sc->frames[0].subtexture->GetWidth(), sc->frames[0].subtexture->GetHeight());
                if (playerRect.containsPoint(mousePos)) {
                    RotateComponent *rc = hero->GetComponent<RotateComponent>();
                    if (rc != NULL) {
                        hero->Remove(rc);
                        sc->rotation = 0;
                    }
                }
            }
        }

        if (InputManager::getInstance()->isKeyPressed(InputManager::KEY_RIGHT)) {
            moveX = 200 * Game::deltaTime;
            SpriteComponent *sc = hero->GetComponent<SpriteComponent>();
            if (sc != nullptr) {
                sc->scale.x = 1;
            }
        }

        if (InputManager::getInstance()->isKeyPressed(InputManager::KEY_LEFT)) {
            moveX = -200 * Game::deltaTime;
            SpriteComponent *sc = hero->GetComponent<SpriteComponent>();
            if (sc != nullptr) {
                sc->scale.x = -1;
            }
        }

        if (InputManager::getInstance()->isKeyPressed(InputManager::KEY_UP)) {
            moveY = (int)(-200 * Game::deltaTime);
        }

        if (InputManager::getInstance()->isKeyPressed(InputManager::KEY_DOWN)) {
            moveY = 200 * Game::deltaTime;
        }

        if (InputManager::getInstance()->isKeyDown(InputManager::KEY_SPACE)) {
            //moveY = -200 * Game::deltaTime;
            Bullet *b = new Bullet();
            SpriteComponent *sc = new SpriteComponent();
            sc->AddFrame(&atlas->subtextures["enemy.png"]);
            sc->AddAnimation(0, true, 1, std::vector<int>{0});
            // Center the origin
            sc->origin.x = sc->frames[0].subtexture->GetSize().x / 2.0f;
            sc->origin.y = sc->frames[0].subtexture->GetSize().y / 2.0f;
            sc->Play(0);
            b->position.x = b->posX = hero->position.x;
            b->position.y = b->posY = hero->position.y;
            b->SetDepth(200);
            b->Add(sc);
            bullets.push_back(b);
            mainScene->Add(b);
            Log::log("space");
        }

      if (InputManager::getInstance()->isKeyDown(InputManager::KEY_Z)) {
        // Spawn a bunny at mouse position
        glm::vec2 mousePos = playerCamera.ScreenToWorldPoint(glm::vec2(InputManager::getInstance()->getMouseX(), InputManager::getInstance()->getMouseY()));
        Bunny *b = new Bunny();
        b->Tag(MyTags::Movables);
        Rectangle wabbitRect = Rectangle(0, 0, wabbit->getWidth(), wabbit->getHeight());
        ImageComponent *ic = new ImageComponent(wabbit, &wabbitRect, true);
        b->Add(ic);
        //Colliders::Circle *c = new Colliders::Circle(wabbit->getSize().x / 2);
        Colliders::Hitbox *c = new Colliders::Hitbox(wabbit->getWidth(), wabbit->getHeight(), 0, 0);
        c->layer = MyLayers::Bunnies;
        //c->position.x = wabbit->getSize().x/2;
        //c->position.y = wabbit->getSize().y/2;
        b->Add(c);
        mainScene->Add(b);
        b->SetDepth(10);
        b->position.x = mousePos.x;
        b->position.y = mousePos.y;
      }

        TextComponent *tc = fps->GetComponent<TextComponent>();
        if (tc != nullptr) {
            tc->text = "FPS: " + String::intToString((fpsC->getFPS()));
            //Log::log(tc->text);
        }
        fpsC->update();


        SpriteComponent *sc = hero->GetComponent<SpriteComponent>();
        /*if (sc != nullptr) {
            sc->rotation += 0.01f;
        }*/

        Rectangle playerRect = Rectangle(hero->position.x - sc->frames[0].subtexture->GetWidth()/2.0f + moveX, hero->position.y - sc->frames[0].subtexture->GetHeight()/2.0f + moveY, sc->frames[0].subtexture->GetWidth(), sc->frames[0].subtexture->GetHeight());
        if (scene->CollideCheck(playerRect, MyLayers::Platforms)) {
            collision = true;
        }


        if (!collision) {
            hero->position.x += moveX;
            hero->position.y += moveY;
        }

        /*if (InputManager::getInstance()->isMouseDown(sf::Mouse::Left)) {
            Vector2 mp = InputManager::getInstance()->getMousePosition(&playerCamera);
            if (Collide::RectToPoint(hero->position.x-8, hero->position.y-8, 16, 16, mp)) {
                Log::log("clicked");
            }
        }

        if (InputManager::getInstance()->isTouchDown(0)) {
            Vector2 tp = InputManager::getInstance()->getTouchPosition(0, &playerCamera);
            if (Collide::RectToPoint(hero->position.x-8, hero->position.y-8, 16, 16, tp)) {
                Log::log("touched");
            }
        }

        if (showTweenBunny) {
            tweenTime += Game::deltaTime;
            if (loopingBunny->position.y == 170) {
                Tween::To(&loopingBunny->position, Vector2(150, 250), 0.5f, EASE_INQUAD);
            } else if (loopingBunny->position.y == 250) {
                Tween::To(&loopingBunny->position, Vector2(150, 170), 0.5f, EASE_INQUAD);
                tweenTime = 0;
            }
        }*/

        /*if (showParticles) {
            particleSystem->Emit(pt, particleSystem->position);
        }*/


        Rectangle checkRect = Rectangle(0, 0, 16, 16);
        float dist;
        Ray2 ray = Ray2(glm::vec2(hero->position.x, hero->position.y), glm::vec2(0, -1000));
        if(Collide::RectToRay(checkRect, ray, dist)) {
            Log::log("ray collision");
        }

        playerRenderer->camera->SetPosition(hero->position.x - Game::window->originalWidth/2, hero->position.y - Game::window->originalHeight/2);

        //Log::log(String::intToString(playerRenderer->camera->GetPosition().y));
        //Vector2 min = playerCamera.ViewportToWorldPoint(Vector2(0, 0));
        //Vector2 max = playerCamera.ViewportToWorldPoint(Vector2(1, 1));

        BuildGui();
    }

    void MyGame::Draw(float dt) {
        Game::Draw(dt);
        /*SDL_Rect r = SDL_Rect();
        r.x = 0;
        r.y = 0;
        r.w = 512;
        r.h = 512;
        SDL_RenderCopy(window->renderer, atlas->texture.mTexture, &r, &r);*/
        //Game::GetSpriteBatch()->DrawString(defaultFont, "FPS: ", 32, Vector2(0, 0), sf::Color::White, Vector2(1, 1), Vector2(0, 0), 0, sf::Text::Style::Regular);

        /*
        for (unsigned long c = 0 ; c < cameras.size() ; c++) {
            if (c == 1) {
                cameras[c].Rotate(cameraAngle);
                cameraAngle += cameraAngleInc;
                if (fabs(cameraAngle) > 3.0f) {
                    cameraAngleInc *= -1;
                }
            }
            window->SetCamera(cameras[c]);
            spriteBatch->Begin();
            for (int32_t i = 0 ; i < bunnies.size() ; i++) {
                Bunny *b = bunnies[i];
                Vector2 *pos = new Vector2(b->posX, b->posY);
                spriteBatch->Draw(wabbit, pos);
                delete(pos);
            }
            spriteBatch->End();
        }
         */
        /*sf::Text t = sf::Text("F", defaultFont, 32);
        t.setPosition(100, 50);
        window->window->draw(t);*/

        /*
        DrawGrid(true);
        for (uint32_t i = 0 ; i < bunnies.size() ; i++) {
            //bunnies[i]->Update(dt);
            Bunny *b = bunnies[i];
            spatialHash->UpdateBody(b->body);
            if( spatialHash->IsBodySharingAnyCell( b->body ) )
            {
                Draw::HollowRect(b->body->aabb.min.x, b->body->aabb.min.y, b->body->aabb.max.x-b->body->aabb.min.x, b->body->aabb.max.y-b->body->aabb.min.y, sf::Color::Green);
            } else {
                Draw::HollowRect(b->body->aabb.min.x, b->body->aabb.min.y, b->body->aabb.max.x-b->body->aabb.min.x, b->body->aabb.max.y-b->body->aabb.min.y, sf::Color::Magenta);
            }
        }
         */
    }

    void MyGame::BuildGui() {
        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        {
            static float f = 0.0f;
            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);
            if (ImGui::Button("Test Window")) show_test_window ^= 1;
            if (ImGui::Button("Another Window")) show_another_window ^= 1;
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        // 2. Show another simple window, this time using an explicit Begin/End pair
        if (show_another_window)
        {
            ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello");
            ImGui::End();
        }

        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        if (show_test_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
            ImGui::ShowTestWindow(&show_test_window);
        }

    }


}
