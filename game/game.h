#ifndef GAME_H
#define GAME_H

#include <atomic>
#include <thread>

#define GLFW_INCLUDE_NONE
#include <render.h>

#include <GLFW/glfw3.h>
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>
#include <manager.h>
#include <GameHelper/camera.h>

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

    Resource::Texture test;
};

#endif
