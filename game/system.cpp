#include "system.h"

Planet::Planet(Resource::Model model, glm::mat4 modelMat, float speed) {
    this->model = model;
    this->modelMat = modelMat;
    this->normMat = glm::inverseTranspose(modelMat);
    this->speed = speed;
}

void Planet::Draw(Render *render) {
    render->DrawModel(model, modelMat, normMat);
}

void Planet::Update(gamehelper::Timer &timer) {
    if(speed != 0) {
	this->modelMat = glm::rotate(this->modelMat, timer.FrameElapsed() * speed * 0.001f,
				  glm::vec3(0, 0, 1));
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
    Resource::Texture sunTex = render->LoadTexture("textures/Planet/sun.png");
    planet.overrideTexID = sunTex.ID;
    sun = Planet(planet, middle, 0.0001f);
    planet.useShading = true;

    //planets
    Resource::Texture planetTex = render->LoadTexture("textures/Planet/planet1.png");
    planet.overrideTexID = planetTex.ID;
    middle = glm::scale(glm::mat4(1.0f),
			glm::vec3(5.0f));
    planets.push_back(
	    Planet(planet, glm::translate(middle, glm::vec3(30, 0, 0)), 0.001f));
    planets.push_back(
	    Planet(planet, glm::translate(middle, glm::vec3(30, 20, 0)), 0.002f));
    planets.push_back(
	    Planet(planet, glm::translate(middle, glm::vec3(-30, -30, 0)), 0.005f));
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