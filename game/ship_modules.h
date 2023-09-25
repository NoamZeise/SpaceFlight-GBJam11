#ifndef SHIP_MODULES_H
#define SHIP_MODULES_H

#include <manager.h>

class Module {
public:
    Module(){}
    Module(Resource::Texture tex, glm::vec4 pos, float depth);
    void Update(gamehelper::Timer &timer);
    virtual void Draw(Render *render);
    glm::vec4 getPos() { return pos; }
    glm::vec2 getDim() { return tex.dim; }
    void setPos(glm::vec4 pos) { this->pos = pos;
	if(pos.z == 0) this->pos.z = tex.dim.x;
	if(pos.w == 0) this->pos.w = tex.dim.y;
	changed = true; }
    void setPos(glm::vec2 pos) { this->pos.x = pos.x; this->pos.y = pos.y;
	changed = true; }
    void setShakeLevel(float level) {
	this->shakeLevel = fabs(level) > 0.2 ? level : 0; }
    void setMaxRot(float rot) { maxRotate = rot; }
    void setTex(Resource::Texture tex) {
	this->tex = tex;
	this->pos.z = tex.dim.x;
	this->pos.w = tex.dim.y;
	changed = true;
    }
    void setBaseRot(float rot) {
	this->baseRot = rot;
	this->rotation = this->baseRot;
	changed = true;
    }

    void setBaseOffset(glm::vec2 off) {
	basePosOffset = off;
	changed = true;
    }

    void updateMat();

private:
    
    Resource::Texture tex;
    float depth;
    glm::mat4 mat;
    bool changed = false;
    glm::vec4 pos;

    float time = 0;
    
    float shakeLevel = 0;
    float rotDir = 1;

    float baseRot = 0;
    float maxRotate = 2.0f;
    
    float rotation = 0;
    float rotVel = 0;
    glm::vec2 basePosOffset = glm::vec2(0);
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

class TargetMod : public Module {
public:
    TargetMod() {}
    TargetMod(Resource::Texture onscreen,
	   Resource::Texture offscreen);
    void Update(gamehelper::Timer &timer,
		glm::mat4 viewprojMat);
    void Draw(Render *render) override;
    void setTarget(glm::vec3 target) {
	this->target = target;
	targeting = true;
    }
    glm::vec3 getTarget() { return target; }
    void clearTarget() {
	targeting = false;
    }
private:
    Resource::Texture onscreen;
    Resource::Texture offscreen;
    bool targeting = false;
    glm::vec3 target;
};


const int SHOW_MESSAGE_DELAY = 3000;
class ShipMessage {
public:
    ShipMessage(){}
    ShipMessage(Render *render);
    void Update(gamehelper::Timer &timer);
    void Draw(Render* render);
    void showMessage(std::string text);
private:
    std::string message;
    float showMessageTimer = SHOW_MESSAGE_DELAY;
    Resource::Font font;
    Resource::Texture bg;
    glm::vec2 off;
    glm::mat4 model;
};

const int DIST_SHOWING_DELAY = 400;
class DistanceDial {
public:
    DistanceDial(){}
    DistanceDial(Render *render);
    void UpdateDial(gamehelper::Timer &timer, TargetMod &mod, Module &dial);
    void Update(glm::vec3 pos, gamehelper::Timer &timer, std::vector<TargetMod> &targets,
		std::vector<TargetMod> &logs);
    void Draw(Render *render);
private:
    Module bg;
    std::vector<Module> dials;
    Module dial;
    glm::vec3 pos;
    glm::vec2 offset;
    bool showing = false;
    float showingTimer = 0;
};


#endif
