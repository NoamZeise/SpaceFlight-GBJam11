#ifndef SHIP_H
#define SHIP_H

#include <manager.h>
#include <GameHelper/camera.h>
#include "controls.h"

#include "ship-modules.h"
#include "collision.h"

class TargetMod : public Module {
public:
    TargetMod() {}
    TargetMod(Resource::Texture onscreen,
	   Resource::Texture offscreen);
    void Update(gamehelper::Timer &timer,
		glm::mat4 viewMat,
		float fov);
    void Draw(Render *render) override;
    void setTarget(glm::vec3 target) {
	this->target = target;
	targeting = true;
    }
    void clearTarget() {
	targeting = false;
    }
private:
    Resource::Texture onscreen;
    Resource::Texture offscreen;
    bool targeting = false;
    glm::vec3 target;
};

const float RESPAWN_DELAY = 3000;

class Ship : public camera::FirstPerson {
 public:
    Ship(){}
    Ship(Render *render, glm::vec3 position);
    void Update(GbInput &input, gamehelper::Timer &timer);
    void Draw(Render *render);

    void PlanetCollision(Collision state, gamehelper::Timer &timer);
    void setSpawn();
    
 private:

    void controls(GbInput &input, gamehelper::Timer &timer);
    
    Module shipGlass;
    Module aim;
    Module ebrakeMod;
    Module criticalDmg;
    Throttle throttle;

    TargetMod target;
    std::vector<TargetMod> targets;
    
    long double shipSpeed = 0.000001;
    float currentSpeed = 0;
    glm::vec3 velocityVec = glm::vec3(0);
    long double throttlePos = 0;
    long double throttleSpeed = 0.001;
    
    glm::vec3 spawn;
    glm::vec3 spawnUp;
    glm::vec3 spawnFront;
    
    bool emergencyBrake = false;
    bool respawning = false;
    float respawnTimer = RESPAWN_DELAY;
};

#endif
