#include "ship-modules.h"

#include "gb_consts.h"
#include <graphics/glm_helper.h>

#include <logger.h>

Module::Module(Resource::Texture tex, glm::vec4 pos, float depth) {
    this->tex = tex;
    this->pos = pos;
    if(this->pos.z == 0)
	this->pos.z = tex.dim.x;
    if(this->pos.w == 0)
	this->pos.w = tex.dim.y;
    this->depth = depth;
    updateMat();
}

void Module::Update(gamehelper::Timer &timer) {
  
    if(shakeLevel > 0)
	rotVel = fabs(shakeLevel) * rotDir * timer.FrameElapsed() * 0.1;
    else
	rotVel = baseRot - rotation;

    if(fabs(rotVel) > 0) {
	rotation += rotVel * timer.FrameElapsed() * 0.01;
	changed = true;
    }

    if(fabs(rotation) > fabs(maxRotate) + fabs(baseRot)) {
	rotDir *= -1;
	rotation = glm::sign(rotation) * maxRotate;
	
    }  
    
    if(posVel.x == 0 && posVel.y == 0) {
	posOffset += posVel;
	changed = true;
    }
    if(changed) {
	changed = false;
	updateMat();
    }
}

void Module::Draw(Render *render) {
    render->DrawQuad(tex, mat);
}

void Module::updateMat() { mat = glmhelper::calcMatFromRect(pos, rotation, depth); }

const int THROTTLE_RANGE = 20;
const int THROTTLE_X = 0;
const int THROTTLE_Y = 72;
const int DIAL_X = THROTTLE_X + 0;
const int DIAL_Y = THROTTLE_Y + 28;

const float SPEED_OFFSET = -4;
const float SPEED_LIM = 1.2;
const int SPEED_DIAL_X_OFF = 8;
const int SPEED_DIAL_Y_OFF = THROTTLE_RANGE;

Throttle::Throttle(Render* render) {
    throttle = Module(
	    render->LoadTexture("textures/ship/throttle.png"),
	    glm::vec4(THROTTLE_X, THROTTLE_Y, 0, 0),
	    GLASS_DEPTH + 0.1f);
    Resource::Texture dial = render->LoadTexture("textures/ship/dial.png");
    throttleDial = Module(dial,
			  glm::vec4(DIAL_X, DIAL_Y, 0, 0),
			  GLASS_DEPTH + 0.15f);
    speedDial = Module(dial, glm::vec4(0),
		       GLASS_DEPTH + 0.15f);
    setSpeedDial(0);
}

void Throttle::Draw(Render *render) {
    throttle.Draw(render);
    throttleDial.Draw(render);
    speedDial.Draw(render);
}

glm::vec4 Throttle::calcDial(long double throttlePos,
			     glm::vec2 offset) {
    glm::vec4 dialPos = throttleDial.getPos();
    return glm::vec4(DIAL_X + offset.x, (DIAL_Y + offset.y) - throttlePos * THROTTLE_RANGE,
		     dialPos.z, dialPos.w);
}

void Throttle::setSpeedDial(float speed) {;
    /*LOG("vel.x: " << velocity.x);
    LOG("vel.y: " << velocity.y);
    LOG("vel.z: " << velocity.z);
    LOG(speed);*/
    if(speed == 0) {
	speedDial.setPos(calcDial(0,
				  glm::vec2(SPEED_DIAL_X_OFF, SPEED_DIAL_Y_OFF)));
    } else {
	float l10 = log10(speed);
	//	LOG(l10);
	l10 = l10 < SPEED_OFFSET ? SPEED_OFFSET: l10;
	double dial = (l10 - SPEED_OFFSET) / ((-SPEED_OFFSET - SPEED_LIM) / 2.0);
	if(dial > THROTTLE_LIM * 2)
	    dial = THROTTLE_LIM * 2;
	    
	speedDial.setPos(calcDial(dial,
			 glm::vec2(SPEED_DIAL_X_OFF, SPEED_DIAL_Y_OFF)));	
    }
    prevSpeed = speed;
}

void Throttle::Update(gamehelper::Timer &timer, long double throttlePos,
		      float speed) {
    if(throttlePos != prevThrottle) {
	throttleDial.setPos(calcDial(throttlePos, glm::vec2(0)));
	prevThrottle = throttlePos;
    }
    if(speed != prevSpeed)
	setSpeedDial(speed);
    throttleDial.Update(timer);
    speedDial.Update(timer);
    throttle.Update(timer);
}
