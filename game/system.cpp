#include "system.h"

const float CLOSE_FACTOR = 1.2f;
const float COLLIDE_FACTOR = 1.01f;

Planet::Planet(std::string name, Resource::Model model, glm::vec3 pos, float radius, float speed) {
    this->name = name;
    this->model = model;
    this->modelMat = glm::scale(glm::translate(glm::mat4(1),
					       pos),
				glm::vec3(radius));
    this->normMat = glm::inverseTranspose(modelMat);
    this->speed = speed;
    this->radius = radius;
    this->pos = pos;
}

void Planet::Draw(Render *render) {
    render->DrawModel(model, modelMat, normMat);
}

void Planet::rotate(float a) {
    this->modelMat = glm::rotate(this->modelMat, a,
				 glm::vec3(0, 0, 1));
}

void Planet::Update(gamehelper::Timer &timer) {
    if(speed != 0) {
	this->rotate(timer.FrameElapsed() * speed * 0.001f);			
	this->normMat = glm::inverseTranspose(this->modelMat);
    }
}

System::System(Render *render) {
    Resource::Model planet = render->Load3DModel("models/planet.obj");

    //sun
    glm::mat4 middle = glm::scale(
	    glm::mat4(1.0f),
	    glm::vec3(30));
    planet.useShading = false;
    Resource::Texture sunTex = render->LoadTexture("textures/Planet/Sun.png");
    planet.overrideTexID = sunTex.ID;
    sun = Planet("sun", planet, glm::vec3(0), 100, 0.0001f);
    planet.useShading = true;

    //planets
    Resource::Texture planetTex = render->LoadTexture("textures/Planet/planet1.png");
    planet.overrideTexID = planetTex.ID;
    middle = glm::scale(glm::mat4(1.0f),
			glm::vec3(5.0f));
    planets.push_back(
	    Planet("sila", planet, glm::vec3(800, 0, -20), 20.0, 0.0025f));
    planets.push_back(Planet("sila-1", planet, glm::vec3(800, 0, 27), 1, 0));
    
    planet.overrideTexID = render->LoadTexture("textures/Planet/metal-planet.png").ID;
    planets.push_back(
	    Planet("RW-1011", planet, glm::vec3(1240, -200, -10), 30.0, 0.001f));

    planet.overrideTexID = render->LoadTexture("textures/Planet/organic1.png").ID;
    planets.push_back(
	    Planet("jomah", planet, glm::vec3(-600, -100, 20), 15.0, 0.005f));

    planet.overrideTexID = render->LoadTexture("textures/Planet/earthlike.png").ID;
    planets.push_back(
	    Planet("gaia", planet,
		   glm::vec3(-1400, 200, 0), 40.5, 0.01f));
    planet.overrideTexID = render->LoadTexture("textures/Planet/moon.png").ID;
    planets.push_back(
	    Planet("diana", planet,
		   glm::vec3(-1400, 50, 10), 6.5, 0.001f));

    
    planet.overrideTexID = render->LoadTexture("textures/Planet/triangles.png").ID;
    planets.push_back(
	    Planet("ripel-1", planet,
		   glm::vec3(890, 1640, -20), 30.5, 0.005f));
    planets.push_back(
	    Planet("ripel-2", planet,
		   glm::vec3(950, 1615, 20), 23.1, 0.005f));
    planets.back().rotate(glm::pi<float>());
    
    /*    planet.overrideTexID = render->LoadTexture("textures/Planet/lines.png").ID;
    planets.push_back(
	    Planet("a", planet,
	    glm::vec3(200, 10, 0), 4.5, 0.1f));
    planet.overrideTexID = render->LoadTexture("textures/Planet/noise.png").ID;
    planets.push_back(
	    Planet("a", planet,
	    glm::vec3(170, 10, 0), 4.5, 0.1f));
    planet.overrideTexID = render->LoadTexture("textures/Planet/moon.png").ID;
    planets.push_back(
	    Planet("mon", planet,
	    glm::vec3(150, 0, 0), 4.5, 0.1f));*/
}

void System::Update(gamehelper::Timer &timer) {
    sun.Update(timer);
    for(auto &p: planets)
	p.Update(timer);
}

void System::Draw(Render *render) {
    sun.Draw(render);
    for(auto &p: planets)
	p.Draw(render);
}

#include <logger.h>

const float SHIP_RAD = 2.0f;

void checkCol(Planet &p, glm::vec3 pos, Collision *col) {
    glm::vec3 point = pos - p.pos;
    float dist = glm::dot(point, point);
    
    float close = ((p.radius + SHIP_RAD) * CLOSE_FACTOR );
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
	checkCol(p, pos, &col);
	if(col.state == CollisionState::Collide)
	    return col;
    }
    checkCol(sun, pos, &col);
    return col;
}
