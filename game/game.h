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

#include "controls.h"
#include "palette.h"

#define TIME_APP_DRAW_UPDATE
//#define MULTI_UPDATE_ON_SLOW_DRAW

class Game {
public:
    Game(RenderFramework defaultFramework);
    ~Game();
    void run();
private:
    void loadAssets();
    void update();
    void controls();
    void postUpdate();
    void draw();

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

    std::vector<Palette> palettes;
    GbInput input;
    Resource::Texture pixelCol0;
    glm::mat4 screenMat;
    
    
    Resource::Texture test;
    glm::vec4 testRect = glm::vec4(0, 0, GB_WIDTH, GB_HEIGHT);
  
};

#endif
