#include "ship.h"

#include "gb_consts.h"
#include <graphics/glm_helper.h>
#include <logger.h>

Ship::Ship(Render *render, glm::vec3 position)
    : camera::FirstPerson(position) {
    shipGlassMat = glmhelper::calcMatFromRect(glm::vec4(0, 0, GB_WIDTH, GB_HEIGHT),
					      0.0f, -0.1f);
    shipGlassTex = render->LoadTexture("textures/ship/glass.png");

    _speed = 0.0001f;
    _front = glm::vec3(-1, 0, 0);
    _up = glm::vec3(0, 0, 1);
    _left = glm::vec3(0, -1, 0);
}

void rotQ(float angle, glm::vec3 axis, glm::vec3 *target, glm::vec3 *correct) {
    if(angle == 0) return;
    glm::quat rot = glm::normalize(glm::quat(angle, axis));
    *target = glm::normalize(rot * *target * glm::conjugate(rot));
    *correct = glm::cross(*target, axis);
}

void Ship::Update(GbInput &input, gamehelper::Timer &timer) {
    float velocity = _speed * timer.FrameElapsed();

    _pitch = _yaw = _roll = 0;
    
    if(input.hold(GB::Up))
	_pitch += velocity;
    if(input.hold(GB::Down))
	_pitch -= velocity;

    if(input.hold(GB::B)) {
	if(input.hold(GB::Left))
	    _yaw -= velocity;
	if(input.hold(GB::Right))
	    _yaw += velocity;
    } else {
	if(input.hold(GB::Left))
	    _roll += velocity;
	if(input.hold(GB::Right))
	    _roll -= velocity;
    }
    
    if(input.hold(GB::A))
	_position += _front * (shipSpeed * timer.FrameElapsed());

    rotQ(_pitch, _left, &_front, &_up);
    rotQ(_yaw, _up, &_left, &_front);
    rotQ(_roll, _front, &_up, &_left);

    viewUpdated = true;
}

void Ship::Draw(Render *render) {
    render->DrawQuad(shipGlassTex, shipGlassMat);
}
