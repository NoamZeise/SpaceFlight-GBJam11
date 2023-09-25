#ifndef SHIP_H
#define SHIP_H

#include <manager.h>
#include <GameHelper/camera.h>
#include "controls.h"

#include "ship_modules.h"
#include "ship_menu.h"
#include "collision.h"
#include "logs.h"

const float RESPAWN_DELAY = 3000;

class Ship : public camera::FirstPerson {
 public:
    Ship(){}
    Ship(Render *render, glm::vec3 position);
    void Update(GbInput &input, gamehelper::Timer &timer);
    void Draw(Render *render);

    void PlanetCollision(Collision state, gamehelper::Timer &timer);
    void LogCollision(Collision state);
    void setSpawn();
    
 private:

    void controls(GbInput &input, gamehelper::Timer &timer);
    void menu(GbInput &input, gamehelper::Timer &timer);
    void physUpdate(gamehelper::Timer &timer);

    enum class ShipState {
	Fly,
	Menu,
    };

    ShipState state = ShipState::Fly;
    
    Module shipGlass;
    Module aim;
    Module ebrakeMod;
    Module criticalDmg;
    Throttle throttle;
    ShipMessage messager;

    MenuMod shipMenu;
    std::vector<TargetMod> targets;
    std::vector<TargetMod> logTargets;

    std::vector<SystemLog> unfoundLogs;
    
    long double shipSpeed = 0.000001;
    float currentSpeed = 0;
    glm::vec3 velocityVec = glm::vec3(0);
    long double throttlePos = 0;
    long double throttleSpeed = 0.001;
    
    glm::vec3 spawn;
    glm::vec3 spawnUp;
    glm::vec3 spawnFront;

    float pitchVel = 0;
    float rollVel = 0;
    float yawVel = 0;
    
    bool emergencyBrake = false;
    bool respawning = false;
    float respawnTimer = RESPAWN_DELAY;
};

#endif
