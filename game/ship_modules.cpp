#include "ship_modules.h"

#include "gb_consts.h"
#include "palette.h"
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

void Module::updateMat() {
    glm::vec4 rect(pos.x + basePosOffset.x + posOffset.x,
		   pos.y + basePosOffset.y + posOffset.y,
		   pos.z, pos.w);
    mat = glmhelper::calcMatFromRect(rect, rotation, depth);
}

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
    if(speed == 0)
	speedDial.setPos(
		calcDial(0, glm::vec2(SPEED_DIAL_X_OFF, SPEED_DIAL_Y_OFF)));
    else {
	float l10 = log10(speed);

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

TargetMod::TargetMod(Resource::Texture onscreen,
		     Resource::Texture offscreen)
    : Module(onscreen, glm::vec4(0), GLASS_DEPTH + 0.15f) {
    this->onscreen = onscreen;
    this->offscreen = offscreen;
}

void correctTarg(float &fixed, float &variable) {
    if(fixed != 0)
	variable /= fabs(fixed);
    fixed = glm::sign(fixed);
}

void TargetMod::Update(gamehelper::Timer &timer,
		       glm::mat4 viewProj) {
    if(!targeting) {
	return;
    }
    glm::vec4 clip = viewProj * glm::vec4(target.x, target.y, target.z, 1);
    
    glm::vec2 screen(clip.x / clip.w, -clip.y / clip.w);
    float dist = clip.z;

    screen *= glm::sign(dist);
    
    if(fabs(screen.x) <= 1 && fabs(screen.y) <= 1 &&
       dist > 0) {
	this->setTex(this->onscreen);
	this->setPos(glm::vec2(((screen.x + 1) / 2) * GB_WIDTH - (this->onscreen.dim.x / 2),
			       ((screen.y + 1) / 2) * GB_HEIGHT - (this->onscreen.dim.y / 2)));
    } else {
	this->setTex(this->offscreen);
	
	if(fabs(screen.x) > fabs(screen.y))
	    correctTarg(screen.x, screen.y);
	else
	    correctTarg(screen.y, screen.x);

	//point in dir of target
	glm::vec2 norm = glm::normalize(screen);
	float angle = atan2(norm.y, norm.x) + (glm::pi<float>() / 2);
	this->setBaseRot(angle * 180 / glm::pi<float>());
	glm::vec2 texPos = glm::vec2(
		((screen.x + 1) / 2) * GB_WIDTH,
		((screen.y + 1) / 2) * GB_HEIGHT);
	if(texPos.x > GB_WIDTH - offscreen.dim.x)
	    texPos.x = GB_WIDTH - offscreen.dim.x;
	if(texPos.y > GB_HEIGHT - offscreen.dim.y)
	    texPos.y = GB_HEIGHT - offscreen.dim.y;
	this->setPos(texPos);
    }

    Module::Update(timer);
}

void TargetMod::Draw(Render *render) {
    if(!targeting)
	return;
    Module::Draw(render);
}


ShipMessage::ShipMessage(Render *render) {
    font = render->LoadFont("textures/dogicapixel.ttf");
    bg = render ->LoadTexture("textures/ship/message.png");
}

const int TRANS_TIME = 300;
const int X_MSG_BORDER = 5;
const int CHAR_W = 7;

void ShipMessage::Update(gamehelper::Timer &timer) {
    if(showMessageTimer < SHOW_MESSAGE_DELAY) {
	showMessageTimer += timer.FrameElapsed();

	float pos = 0;
	if(showMessageTimer < TRANS_TIME) {
	    pos = 1 - (showMessageTimer / TRANS_TIME);
	} else if(showMessageTimer > SHOW_MESSAGE_DELAY - TRANS_TIME) {
	    pos = 1 - ((SHOW_MESSAGE_DELAY - showMessageTimer)/TRANS_TIME);
	}
	int w = message.size() * CHAR_W + X_MSG_BORDER * 2;
	off = glm::vec2(
		(GB_WIDTH - w)/2 , (pos * -40));
	model = glmhelper::calcMatFromRect(glm::vec4(off.x, off.y, w, bg.dim.y),
					   GLASS_DEPTH + 0.24f);
    }
}

void ShipMessage::Draw(Render *render) {
    if(showMessageTimer < SHOW_MESSAGE_DELAY) {
	render->DrawQuad(bg, model);
	render->DrawString(font, message,
			   glm::vec2(off.x + X_MSG_BORDER, off.y + 16)
			   , 1, GLASS_DEPTH + 0.25f, COL1);
    }
}

void ShipMessage::showMessage(std::string text) {
    if(showMessageTimer < SHOW_MESSAGE_DELAY &&
       showMessageTimer > TRANS_TIME) {
	showMessageTimer = TRANS_TIME;
    } else if(text != message) {
	showMessageTimer = 0;
    }
    message = text;
}
