#ifndef GAME_H
#define GAME_H

#define GLFW_INCLUDE_NONE
#include <render.h>
#include <GLFW/glfw3.h>
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>

#include <manager.h>
#include <GameHelper/camera.h>

#include <vector>
#include <atomic>
#include <thread>

#include "menu.h"
#include "system.h"
#include "ship.h"

#define TIME_APP_DRAW_UPDATE
//#define MULTI_UPDATE_ON_SLOW_DRAW

enum class GameState {
    Menu,
    Game,
    Debug
};

class Game {
public:
    Game(RenderFramework defaultFramework);
    ~Game();
    void run();
private:
    void loadAssets();
    void update();
    void postUpdate();
    void draw();

    void menuUpdate();
    void gameUpdate();

    Manager* manager;
    camera::FirstPerson fpcam;

    glm::vec4 lightDir = glm::vec4(0.0f, -0.5f, -1.0f, 0.0f);

    std::thread submitDraw;
    std::atomic<bool> finishedDrawSubmit;
    std::thread assetLoadThread;
    std::atomic<bool> assetsLoaded;

#ifdef TIME_APP_DRAW_UPDATE
    std::string monitored_update_stats = "";
    std::string  monitored_draw_stats = "";
#endif

    GameState state = GameState::Game;
    
    GbInput input;
    MainMenu menu;
    Resource::Texture pixel;
    glm::mat4 screenMat;
    
    Resource::Font gameFont;
    
    System system;
    Ship ship;
    Resource::Model stars;
    glm::mat4 starModel;
};

#endif
