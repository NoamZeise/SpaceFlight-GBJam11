#ifndef SHIP_H
#define SHIP_H

#include <manager.h>
#include <GameHelper/camera.h>
#include "controls.h"

#include "ship-modules.h"

class TargetMod : public Module {
public:
    TargetMod() {}
    TargetMod(Resource::Texture onscreen,
	   Resource::Texture offscreen);
    void Update(gamehelper::Timer &timer,
		glm::vec3 pos,
		glm::vec3 front);
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

class Ship : public camera::FirstPerson {
 public:
    Ship(){}
    Ship(Render *render, glm::vec3 position);
    void Update(GbInput &input, gamehelper::Timer &timer);
    void Draw(Render *render);
 private:

    void controls(GbInput &input, gamehelper::Timer &timer);
    
    Module shipGlass;
    Module aim;
    Throttle throttle;

    TargetMod target;
    
    long double shipSpeed = 0.000001;
    float currentSpeed = 0;
    glm::vec3 velocityVec = glm::vec3(0);
    long double throttlePos = 0;
    long double throttleSpeed = 0.001;
};

#endif
