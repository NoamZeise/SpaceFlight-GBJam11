#include "ship.h"

#include "gb_consts.h"
#include <graphics/glm_helper.h>
#include <logger.h>

const float BASE_FOV = 40.0f;
const float THROTTLE_FOV = 8.0f;
const float SPEED_FOV = 400.0f;

void logVec3(glm::vec3 vec) {
    LOG("x: " << vec.x << " , y: " << vec.y <<
	" , z: " << vec.z);
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
		       glm::mat4 viewMat,
		       float fov) {
    if(!targeting) {
	return;
    }
    glm::mat4 proj = glm::perspective(glm::radians(fov),
				      (float)GB_WIDTH / (float)GB_HEIGHT,
				      NEAR_CLIP_3D, FAR_CLIP_3D);
    glm::vec4 clip = proj * viewMat * glm::vec4(target.x, target.y, target.z, 1);
    
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

Ship::Ship(Render *render, glm::vec3 position)
    : camera::FirstPerson(position) {
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

    target = TargetMod(render->LoadTexture("textures/ship/target-onscreen.png"),
		       render->LoadTexture("textures/ship/target-offscreen.png"));

    targets.push_back(target);
    targets.back().setTarget(glm::vec3(-400, 200, 0));

    _speed = 0.0001f;
    _front = glm::vec3(-1, 0, 0);
    _up = glm::vec3(0, 0, 1);
    _left = glm::vec3(0, -1, 0);
    
    setSpawn();
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

void Ship::controls(GbInput &input, gamehelper::Timer &timer) {
    float sp = _speed * timer.FrameElapsed(); 

    float pitch = 0, yaw = 0, roll = 0;
    
    float* updown = &pitch;
    float* leftright = &roll;
    float lrfactor = 1.4;

    //alt controls
    /*if(input.hold(GB::B)) {
	leftright = &yaw;
	lrfactor = -1;
	throttleSpeed *= -1;
	}*/
    
    if(input.hold(GB::A)) {
	throttlePos += timer.FrameElapsed() * throttleSpeed;
    }
    if(input.hold(GB::B)) {
	throttlePos -= timer.FrameElapsed() * throttleSpeed;
    }
    
    throttlePos = throttlePos > THROTTLE_LIM ? THROTTLE_LIM
	: throttlePos < -THROTTLE_LIM ? -THROTTLE_LIM
	: throttlePos;

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
    updateView();
}

const float EBRAKE_MIN_SPEED = 0.001f;

void Ship::Update(GbInput &input, gamehelper::Timer &timer) {
    if(!respawning)
	controls(input, timer);

    float additionalFov = (throttlePos > 0
			   ? (throttlePos * THROTTLE_FOV) : 0)
	+ (currentSpeed * SPEED_FOV);
    _zoom = additionalFov + BASE_FOV;    
    
    shipGlass.setShakeLevel(throttlePos);
    shipGlass.Update(timer);
    aim.setShakeLevel(throttlePos);
    aim.Update(timer);
    for(auto& t: targets)
	t.Update(timer, view, _zoom);
    throttle.Update(timer, throttlePos, currentSpeed);
    if(emergencyBrake) {
	ebrakeMod.Update(timer);
    }
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
    if(emergencyBrake)
	ebrakeMod.Draw(render);
    aim.Draw(render);
    for(auto& t: targets)
	t.Draw(render);
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

