#ifndef SHIP_MODULES_H
#define SHIP_MODULES_H

#include <manager.h>

class Module {
public:
    Module(){}
    Module(Resource::Texture tex, glm::vec4 pos, float depth);
    void Update(gamehelper::Timer &timer);
    void Draw(Render *render);
    glm::vec4 getPos() { return pos; }
    void setPos(glm::vec4 pos) { this->pos = pos; changed = true; }
    void setShakeLevel(float level) {
	this->shakeLevel = abs(level) > 0.2 ? level : 0; }
    void setMaxRot(float rot) { maxRotate = rot; }
private:

    void updateMat();
    
    Resource::Texture tex;
    float depth;
    glm::mat4 mat;
    bool changed = false;
    glm::vec4 pos;

    float time = 0;
    
    float shakeLevel = 0;
    float rotDir = 1;

    float maxRotate = 2.0f;
    
    float rotation = 0;
    float rotVel = 0;
    glm::vec2 posOffset = glm::vec2(0);
    glm::vec2 posVel = glm::vec2(0);
};

class Throttle {
public:
    Throttle() {}
    Throttle(Render *render);
    void Update(gamehelper::Timer &timer, long double throttlePos,
		float speed);
    void Draw(Render *render);

private:
    glm::vec4 calcDial(long double throttlePos,
		       glm::vec2 offset);
    void setSpeedDial(float speed);
    
    Module throttle;
    Module throttleDial;
    Module speedDial;
    long double prevThrottle = 0;
    float prevSpeed = 0;
};

#endif
