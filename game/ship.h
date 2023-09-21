#ifndef SHIP_H
#define SHIP_H

#include <manager.h>
#include <GameHelper/camera.h>
#include "controls.h"

class Ship : public camera::FirstPerson {
 public:
    Ship(){}
    Ship(Render *render, glm::vec3 position);
    void Update(GbInput &input, gamehelper::Timer &timer);
    void Draw(Render *render);
 private:
    Resource::Texture shipGlassTex;
    glm::mat4 shipGlassMat;
    float shipSpeed = 0.01f;
};

#endif
