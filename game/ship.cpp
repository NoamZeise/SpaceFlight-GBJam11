#include "ship.h"

#include "gb_consts.h"
#include <graphics/glm_helper.h>
#include <logger.h>
#include <fstream>

void logVec3(glm::vec3 vec) {
    LOG("x: " << vec.x << " , y: " << vec.y <<
	" , z: " << vec.z);
}

const float BASE_FOV = 40.0f;
const float THROTTLE_FOV = 8.0f;
const float SPEED_FOV = 400.0f;

const float ROT_SPEED = 0.0000001f;
const float ROT_MAX = 0.001f;


Ship::Ship(Render *render)
    : camera::FirstPerson(glm::vec3(600, 0, 0)) {
    shipGlass = Module(render->LoadTexture("textures/ship/glass.png"),
		       glm::vec4(0, 0, GB_WIDTH, GB_HEIGHT),
		       GLASS_DEPTH);
    aim = Module(render->LoadTexture("textures/ship/aim.png"),
		 glm::vec4((GB_WIDTH / 2) - 3, (GB_HEIGHT / 2) - 3, 0, 0),
		 GLASS_DEPTH);
    aim.setMaxRot(0.0f);

    ebrakeMod = Module(render->LoadTexture("textures/ship/ebrake.png"),
		       glm::vec4(60, 70, 0, 0),
		       GLASS_DEPTH + 0.1f);
    ebrakeMod.setShakeLevel(1.0);

    auto critTex = render->LoadTexture("textures/ship/criticaldmg.png");
    criticalDmg = Module(critTex,
			 glm::vec4(-critTex.dim.x/6, -critTex.dim.y/6, 0, 0),
			 GLASS_DEPTH - 0.01f);
    criticalDmg.setBaseRot(5.0f);
    criticalDmg.setShakeLevel(1);
    
    throttle = Throttle(render);
    shipMenu = MenuMod(render);
    distDial = DistanceDial(render);
    messager = ShipMessage(render);

    unfoundLogs = getLogs();
    for(int i = 0; i < unfoundLogs.size(); i++) {
	if(unfoundLogs[i].found) {
	    shipMenu.addLog(unfoundLogs[i], true);
	    unfoundLogs.erase(unfoundLogs.begin() + i--);
	}
    }
    _position = getPlayerSpawn();
    logVec3(_position);
    _speed = ROT_SPEED;
    _front = glm::vec3(-1, 0, 0);
    _up = glm::vec3(0, 0, 1);
    _left = glm::vec3(0, -1, 0);

    shipCreated = true;
    
    setSpawn();
}

void Ship::SaveGame() {
    if(shipCreated) {
	std::ofstream f("ship.save", std::ofstream::trunc);
	if(f.good()) {
	    f << _position.x << " " << _position.y << " " << _position.z << " \n";
	    for(auto &l: unfoundLogs) {
		f << l.title << "\n";
	    }
	    f.close();
	}
    }
}

void Ship::setSpawn() {
    spawn = _position;
    spawnFront = _front;
    spawnUp = _up;
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

float limRot(float *rot) {
    if(fabs(*rot) > ROT_MAX)
	*rot = glm::sign(*rot) * ROT_MAX;
    return *rot;
}

void Ship::controls(GbInput &input, gamehelper::Timer &timer) {
    float sp = _speed * timer.FrameElapsed(); 

    float pitch = 0, yaw = 0, roll = 0;
    
    float* updown = &pitchVel;
    float* leftright = &rollVel;
    const float lrfactor = 1.0;
    
    if(input.hold(GB::A))
	throttlePos += timer.FrameElapsed() * throttleSpeed;
    if(input.hold(GB::B))
	throttlePos -= timer.FrameElapsed() * throttleSpeed;
    
    throttlePos = throttlePos > THROTTLE_LIM ? THROTTLE_LIM
	: throttlePos < -THROTTLE_LIM ? -THROTTLE_LIM
	: throttlePos;
    
    if(input.hold(GB::Up))
	*updown += sp;
    if(input.hold(GB::Down))
	*updown -= sp;

    if(input.hold(GB::Left))
	*leftright += sp * lrfactor;
    if(input.hold(GB::Right))
	*leftright -= sp * lrfactor;

    if(input.press(GB::Select)) {
	state = ShipState::Menu;
	shipMenu.toggleMenu();
    }
}

void Ship::menu(GbInput &input, gamehelper::Timer &timer) {
    if(input.press(GB::Select) || shipMenu.isExit()) {
	shipMenu.toggleMenu();
	state = ShipState::Fly;
    }
    shipMenu.Update(input, timer, &targets);
}

void Ship::physUpdate(gamehelper::Timer &timer) {
    if(throttlePos > 0) {
	velocityVec += mulv3(throttlePos * throttlePos * shipSpeed * timer.FrameElapsed(), _front);
	currentSpeed = glm::length(velocityVec);
    }
    else if(throttlePos < 0) {
	velocityVec += mulv3(timer.FrameElapsed() * shipSpeed * 500 *
			     -(throttlePos * throttlePos), velocityVec);
	currentSpeed = glm::length(velocityVec);
    }
	    
    _position += mulv3(timer.FrameElapsed(), velocityVec);
    rotQ(limRot(&pitchVel) * timer.FrameElapsed(), _left, &_front, &_up);
    rotQ(limRot(&yawVel) * timer.FrameElapsed(), _up, &_left, &_front);
    rotQ(limRot(&rollVel) * timer.FrameElapsed(), _front, &_up, &_left);

    float additionalFov = (throttlePos > 0
			   ? (throttlePos * THROTTLE_FOV) : 0)
	+ (currentSpeed * SPEED_FOV);
    _zoom = additionalFov + BASE_FOV;  
	 
    viewUpdated = true;
    updateView();
}

const float EBRAKE_MIN_SPEED = 0.001f;

void Ship::Update(GbInput &input, gamehelper::Timer &timer) {
    if(!respawning) {
	switch(state) {
	case ShipState::Fly:
	    if(shipMenu.menuInTransition())
		shipMenu.Update(input, timer, &targets);
	    controls(input, timer);
	    break;
	case ShipState::Menu:
	    menu(input, timer);
	    break;
	}
	physUpdate(timer);
    }  
    messager.Update(timer);
    shipGlass.setShakeLevel(throttlePos);
    shipGlass.Update(timer);
    distDial.Update(_position, timer, targets, logTargets);
    aim.setShakeLevel(throttlePos);
    aim.Update(timer);
    if(targets.size() > 0 || logTargets.size() > 0) {
	glm::mat4 viewproj =
	    glm::perspective<float>(
		glm::radians(_zoom),
		(float)GB_WIDTH / (float)GB_HEIGHT,
		NEAR_CLIP_3D, FAR_CLIP_3D)
	    * view;
	for(auto& t: targets)
	    t.Update(timer, viewproj);
	for(auto& t: logTargets)
	    t.Update(timer, viewproj);
    }
    throttle.Update(timer, throttlePos, currentSpeed);
    if(emergencyBrake)
	ebrakeMod.Update(timer);
    if(respawning) {
	respawnTimer += timer.FrameElapsed();
	criticalDmg.Update(timer);
	if(respawnTimer > RESPAWN_DELAY) {
	    respawning = false;
	    _position = spawn;
	    velocityVec = glm::vec3(0);
	    throttlePos = 0;
	    currentSpeed = 0;
	    _front = spawnFront;
	    _up = spawnUp;
	    _left = glm::cross(_up, _front);
	    pitchVel = rollVel = yawVel = 0;
	    respawning = false;
	}
    }
    emergencyBrake = false;
}

void Ship::Draw(Render *render) {
    if(respawnTimer < RESPAWN_DELAY)
	criticalDmg.Draw(render);
    shipGlass.Draw(render);
    throttle.Draw(render);
    distDial.Draw(render);
    if(emergencyBrake)
	ebrakeMod.Draw(render);
    aim.Draw(render);
    for(auto& t: targets)
	t.Draw(render);
    for(auto& t: logTargets)
	t.Draw(render);
    messager.Draw(render);
    if(state == ShipState::Menu || shipMenu.menuInTransition())
	shipMenu.Draw(render);
}

void Ship::PlanetCollision(Collision collision, gamehelper::Timer &timer) {
    glm::vec3 dir;
    float angle;
    switch(collision.state) {
    case CollisionState::None: return;
    case CollisionState::Close:
	dir = collision.pos - _position;
	angle = acos(glm::dot(glm::normalize(dir), glm::normalize(velocityVec)));
	emergencyBrake = true;
	if(angle * 2 < 2.4 && currentSpeed > EBRAKE_MIN_SPEED) {
	    angle = 0.05f * timer.FrameElapsed();
	    velocityVec -= velocityVec * (angle < 0.9 ? angle : 0.9f);
	}
	break;
    case CollisionState::Collide:
	velocityVec = glm::vec3(0);
	emergencyBrake = true ;
	if(!respawning) {
	    respawnTimer = 0;
	    respawning = true;
	}
	
	break;
    }
}

void Ship::LogCollision(Collision c) {
    switch (c.state) {
    case CollisionState::Collide:
	for(int i = 0; i < unfoundLogs.size(); i++)
	    if(c.pos == unfoundLogs[i].pos) {
		shipMenu.addLog(unfoundLogs[i], false);
		for(int j = 0; j < logTargets.size(); j++) {
		    if(logTargets[j].getTarget() == c.pos) {
			logTargets.erase(logTargets.begin() + j--);
			break;
		    }
		}
		unfoundLogs.erase(unfoundLogs.begin() + i--);
		setSpawn();
		messager.showMessage("Log Found");
		SaveGame();
	    }
	break;
    case CollisionState::Close:
	for(int i = 0; i < logTargets.size(); i++) {
	    if(logTargets[i].getTarget() == c.pos)
		return;
	}
	messager.showMessage("Log Near");
	logTargets.push_back(shipMenu.getTargetMod());
	logTargets.back().setTarget(c.pos);
	break;
    }
}

