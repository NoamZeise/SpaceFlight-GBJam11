#include "ship.h"

#include "gb_consts.h"
#include <graphics/glm_helper.h>
#include <logger.h>

TargetMod::TargetMod(Resource::Texture onscreen,
		     Resource::Texture offscreen)
    : Module(onscreen, glm::vec4(0), GLASS_DEPTH + 0.15f) {
    this->onscreen = onscreen;
    this->offscreen = offscreen;
}

void TargetMod::Update(gamehelper::Timer &timer,
		       glm::vec3 pos,
		       glm::vec3 front) {
    Module::Update(timer);

    glm::vec3 tg = glm::normalize(target - pos);
    float angle = acos(glm::dot(glm::normalize(front), tg));
    LOG("angle: " << angle);
		       
}

Ship::Ship(Render *render, glm::vec3 position)
    : camera::FirstPerson(position) {
    shipGlass = Module(render->LoadTexture("textures/ship/glass.png"),
		       glm::vec4(0, 0, GB_WIDTH, GB_HEIGHT),
		       GLASS_DEPTH);
    aim = Module(render->LoadTexture("textures/ship/aim.png"),
		 glm::vec4((GB_WIDTH / 2) - 3, (GB_HEIGHT / 2) - 3, 0, 0),
		 GLASS_DEPTH);
    aim.setMaxRot(0.0f);
    throttle = Throttle(render);

    target = TargetMod(render->LoadTexture("textures/ship/target-onscreen.png"),
		       render->LoadTexture("textures/ship/target-offscreen.png"));

    target.setTarget(glm::vec3(800, 0, 27));

    _speed = 0.0001f;
    _front = glm::vec3(-1, 0, 0);
    _up = glm::vec3(0, 0, 1);
    _left = glm::vec3(0, -1, 0);
}

glm::vec3 mulv3(long double scalar, glm::vec3 vec) {
    return glm::vec3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

void rotQ(float angle, glm::vec3 axis, glm::vec3 *target, glm::vec3 *correct) {
    if(angle == 0) return;
    glm::quat rot = glm::normalize(glm::quat(angle, axis));
    *target = glm::normalize(rot * *target * glm::conjugate(rot));
    *correct = glm::cross(*target, axis);
}

void Ship::controls(GbInput &input, gamehelper::Timer &timer) {
    float sp = _speed * timer.FrameElapsed();

    float pitch = 0, yaw = 0, roll = 0;
    
    float* updown = &pitch;
    float* leftright = &roll;
    float lrfactor = 1.4;
    float throttleSpeed = this->throttleSpeed;

    //alt controls
    if(input.hold(GB::B)) {
	leftright = &yaw;
	lrfactor = -1;
	throttleSpeed *= -1;
    }
    
    if(input.hold(GB::A)) {
	throttlePos += timer.FrameElapsed() * throttleSpeed;
	throttlePos = throttlePos > THROTTLE_LIM ? THROTTLE_LIM
	    : throttlePos < -THROTTLE_LIM ? -THROTTLE_LIM
	    : throttlePos;
    }

    if(throttlePos > 0) {
	velocityVec += mulv3(throttlePos * throttlePos * shipSpeed * timer.FrameElapsed(), _front);
	currentSpeed = glm::length(velocityVec);
    }
    else if(throttlePos < 0) {
	velocityVec += mulv3(timer.FrameElapsed() * shipSpeed * 500 *
			     -(throttlePos * throttlePos), velocityVec);
	currentSpeed = glm::length(velocityVec);
    }

    //LOG("speed: " << currentSpeed);

    //LOG("x: " << velocityVec.x << " , y: " << velocityVec.y <<
    //	" , z: " << velocityVec.z);
    
    if(input.hold(GB::Up))
	*updown += sp;
    if(input.hold(GB::Down))
	*updown -= sp;

    if(input.hold(GB::Left))
	*leftright += sp * lrfactor;
    if(input.hold(GB::Right))
	*leftright -= sp * lrfactor;


    _position += mulv3(timer.FrameElapsed(), velocityVec);

    //LOG("pos.x: " << _position.x << ", pos.y: " << _position.y);

    rotQ(pitch, _left, &_front, &_up);
    rotQ(yaw, _up, &_left, &_front);
    rotQ(roll, _front, &_up, &_left);
    
    viewUpdated = true;
}

void Ship::Update(GbInput &input, gamehelper::Timer &timer) {
    controls(input, timer);
    shipGlass.setShakeLevel(throttlePos);
    shipGlass.Update(timer);
    aim.setShakeLevel(throttlePos);
    aim.Update(timer);
    target.Update(timer, _position, _front);
    throttle.Update(timer, throttlePos, currentSpeed);

    //FIX ME
    _zoom = (currentSpeed * 2500) + 40.0;
}

void Ship::Draw(Render *render) {
    shipGlass.Draw(render);
    throttle.Draw(render);
    aim.Draw(render);
    target.Draw(render);
}
