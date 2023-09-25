#include "system.h"

#include <GameHelper/random.h>
#include "system_consts.h"
#include "logs.h"


const float CLOSE_FACTOR = 1.2f;
const float COLLIDE_FACTOR = 1.05f;

Planet::Planet(std::string name, Resource::Model model, glm::vec3 pos, float radius, float speed) {
    this->name = name;
    this->model = model;
    this->modelMat = glm::scale(glm::translate(glm::mat4(1),
					       pos),
				glm::vec3(radius));
    this->speed = speed;
    this->radius = radius;
    this->pos = pos;
    rotate(0);
}

void Planet::Draw(Render *render) {
    render->DrawModel(model, modelMat, normMat);
}

void Planet::rotate(float a) {
    this->modelMat = glm::rotate(this->modelMat, a, axisOfRot);
    this->normMat = glm::inverseTranspose(this->modelMat);
}

void Planet::Update(gamehelper::Timer &timer) {
    if(speed != 0)
	this->rotate(timer.FrameElapsed() * speed * 0.001f);
}

const float LOG_SCALE = 0.1;

LogModel::LogModel(Resource::Model model, glm::vec3 pos)
    : Planet("", model, pos, LOG_SCALE, gamehelper::random::real()) {
    axisOfRot = glm::vec3(gamehelper::random::real(), gamehelper::random::real(), gamehelper::random::real());
}

System::System(Render *render) {
    Resource::Model planet = render->Load3DModel("models/planet.obj");
    
    for(auto &t: SYSTEM_PLANETS) {
	planet.useShading = t.shading;
	if(t.tex != "")
	    planet.overrideTexID = render->LoadTexture(t.tex).ID;
	else planet.overrideTexID = -1;
	planets.push_back(
		Planet(t.name, planet, t.pos, t.radius, t.speed)
			  );
	planets.back().rotate(t.initalRot);
    }

    Resource::Model logModel = render->Load3DModel("models/log.obj");
    std::vector<SystemLog> logs = getLogs();
    for(auto& l: logs) {
	if(!l.found)
	    this->logs.push_back(LogModel(logModel, l.pos));
    }
}

void System::Update(gamehelper::Timer &timer) {
    for(auto &p: planets)
	p.Update(timer);
    for(auto& l: logs) {
	l.Update(timer);
    }
}

void System::Draw(Render *render) {
    for(auto &p: planets)
	p.Draw(render);
    for(auto &l: logs)
	l.Draw(render);
}

const float SHIP_RAD = 3.0f;

void checkCol(Planet &p, glm::vec3 pos, Collision *col,
	      float closeOffset) {
    glm::vec3 point = pos - p.pos;
    float dist = glm::dot(point, point);
    float close = ((p.radius + SHIP_RAD) * CLOSE_FACTOR ) + closeOffset;
    if(dist < close * close)
	col->state = CollisionState::Close;
    else
	return;
    col->pos = p.pos;
    float collide = (p.radius * COLLIDE_FACTOR) + SHIP_RAD/1.25f;
    if(dist < collide * collide)
	col->state = CollisionState::Collide;
}

Collision System::planetCollisions(glm::vec3 pos) {
    Collision col = {CollisionState::None, glm::vec3(0) };
    for(auto& p: planets) {
	checkCol(p, pos, &col, 0);
	if(col.state == CollisionState::Collide)
	    return col;
    }
    return col;
}

Collision System::logCollisions(glm::vec3 pos) {
    Collision col = {CollisionState::None, glm::vec3(0) };
    for(int i = 0; i < logs.size(); i++) {
	checkCol(logs[i], pos, &col, 350);
	if(col.state == CollisionState::Collide) {
	    logs.erase(logs.begin() + i--);
	    return col;
	}
    }
    return col;
}
